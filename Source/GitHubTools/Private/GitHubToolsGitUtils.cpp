#include "GitHubToolsGitUtils.h"

#include "GitHubTools.h"
#include "GitSourceControlModule.h"
#include "GitSourceControlUtils.h"
#include "HttpRequests/GitHubToolsHttpRequest_GetPullRequestFilePatches.h"
#include "HttpRequests/GitHubToolsHttpRequest_GetPullRequestInfos.h"
#include "HttpRequests/GitHubToolsHttpRequest_MarkFileAsViewed.h"
#include "Widgets/SGitHubToolsFilePatch.h"

#include <AssetDefinition.h>
#include <AssetRegistry/AssetRegistryModule.h>
#include <AssetToolsModule.h>
#include <HttpRequests/GitHubToolsHttpRequest_GetPullRequestFiles.h>

#define LOCTEXT_NAMESPACE "GitHubTools.Requests"

namespace GitHubToolsUtils
{
    TOptional< FAssetData > GetAssetDataFromFileInfos( const FGithubToolsPullRequestFileInfos & file_infos )
    {
        const auto & git_source_control = FModuleManager::GetModuleChecked< FGitSourceControlModule >( "GitSourceControl" );
        const auto & path_to_repository_root = git_source_control.GetProvider().GetPathToRepositoryRoot();

        const auto absolute_path = FPaths::ConvertRelativePathToFull( path_to_repository_root, file_infos.Path );

        if ( FString package_name;
             FPackageName::TryConvertFilenameToLongPackageName( absolute_path, package_name ) )
        {
            TArray< FAssetData > assets;
            FModuleManager::LoadModuleChecked< FAssetRegistryModule >( TEXT( "AssetRegistry" ) ).Get().GetAssetsByPackageName( *package_name, assets );
            if ( assets.Num() == 1 )
            {
                return assets[ 0 ];
            }
        }

        return TOptional< FAssetData >();
    }

    void DiffTextFile( const FGithubToolsPullRequestFileInfosPtr & file_infos )
    {
        const TSharedPtr< SWindow > window = SNew( SWindow )
                                                 .Title( LOCTEXT( "DiffTextWindowTitle", "Text diff" ) )
                                                 .ClientSize( FVector2D( 1000, 800 ) );

        window->SetContent( SNew( SGitHubToolsFilePatch )
                                .FileInfos( file_infos )
                                .ParentWindow( window ) );

        if ( const TSharedPtr< SWindow > active_modal = FSlateApplication::Get().GetActiveModalWindow();
             active_modal.IsValid() )
        {
            FSlateApplication::Get().AddWindowAsNativeChild( window.ToSharedRef(), active_modal.ToSharedRef() );
        }
        else
        {
            FSlateApplication::Get().AddWindow( window.ToSharedRef() );
        }
    }

    void DiffUAsset( const FGithubToolsPullRequestFileInfos & file_infos )
    {
        auto optional_asset_data = GetAssetDataFromFileInfos( file_infos );
        if ( !optional_asset_data.IsSet() )
        {
            return;
        }

        const auto & git_source_control = FModuleManager::GetModuleChecked< FGitSourceControlModule >( "GitSourceControl" );
        const auto & path_to_repository_root = git_source_control.GetProvider().GetPathToRepositoryRoot();
        const auto & status_branch_names = FGitSourceControlModule::Get().GetProvider().GetStatusBranchNames();
        const auto & branch_name = status_branch_names[ 0 ];
        const auto & path_to_git_binary = git_source_control.AccessSettings().GetBinaryPath();

        auto asset_data = optional_asset_data.GetValue();

        const auto package_path = asset_data.PackageName.ToString();
        const auto package_name = asset_data.AssetName.ToString();
        auto * current_object = asset_data.GetAsset();

        // Get the file name of package
        FString relative_file_name;
        if ( !FPackageName::DoesPackageExist( package_path, &relative_file_name ) )
        {
            return;
        }

        TArray< FString > errors;
        const auto & revision = GitSourceControlUtils::GetOriginRevisionOnBranch( path_to_git_binary, path_to_repository_root, relative_file_name, errors, branch_name );

        check( revision.IsValid() );

        FString temp_file_name;
        if ( !revision->Get( temp_file_name ) )
        {
            return;
        }

        auto * temp_package = LoadPackage( nullptr, *temp_file_name, LOAD_ForDiff | LOAD_DisableCompileOnLoad );
        if ( temp_package == nullptr )
        {
            return;
        }

        auto * old_object = FindObject< UObject >( temp_package, *package_name );
        if ( old_object == nullptr )
        {
            return;
        }

        FRevisionInfo old_revision;
        old_revision.Changelist = revision->GetCheckInIdentifier();
        old_revision.Date = revision->GetDate();
        old_revision.Revision = revision->GetRevision();

        FRevisionInfo new_revision;
        new_revision.Revision = TEXT( "" );

        FModuleManager::GetModuleChecked< FAssetToolsModule >( "AssetTools" ).Get().DiffAssets( old_object, current_object, old_revision, new_revision );
    }

    void DiffFileAgainstOriginStatusBranch( const FGithubToolsPullRequestFileInfosPtr & file_infos )
    {
        if ( file_infos == nullptr )
        {
            return;
        }

        if ( !file_infos->IsUAsset() )
        {
            DiffTextFile( file_infos );
        }
    }

    void DiffFilesAgainstOriginStatusBranch( const TArray< FGithubToolsPullRequestFileInfosPtr > & file_infos )
    {
        for ( auto file : file_infos )
        {
            DiffFileAgainstOriginStatusBranch( file );
        }
    }

    TFuture< FGithubToolsPullRequestInfosPtr > GetPullRequestInfos( int pr_number )
    {
        struct FWrapper
        {
            TPromise< FGithubToolsPullRequestInfosPtr > Promise;
        };

        // Not great to have a static member, but we need a promise that is not destroyed when the function exits
        static TSharedPtr< FWrapper > wrapper;

        wrapper = MakeShared< FWrapper >();

        FGitHubToolsModule::Get()
            .GetRequestManager()
            .SendPaginatedRequest< FGitHubToolsHttpRequestData_GetPullRequestFiles >( pr_number )
            .Then( [ &, pr_number ]( TFuture< TArray< FGithubToolsPullRequestFileInfosPtr > > pr_files ) {
                auto files = pr_files.Get();

                FGitHubToolsModule::Get()
                    .GetRequestManager()
                    .SendRequest< FGitHubToolsHttpRequestData_GetPullRequestFilePatches >( pr_number )
                    .Then( [ &, files = MoveTemp( files ), pr_number ]( const TFuture< FGitHubToolsHttpRequestData_GetPullRequestFilePatches > & file_patches ) {
                        auto patches = file_patches.Get().GetResult().GetValue();

                        FGitHubToolsModule::Get()
                            .GetRequestManager()
                            .SendRequest< FGitHubToolsHttpRequestData_GetPullRequestInfos >( pr_number, files, MoveTemp( patches ) )
                            .Then( [ & ]( const TFuture< FGitHubToolsHttpRequestData_GetPullRequestInfos > & get_pr_infos ) {
                                wrapper->Promise.SetValue( get_pr_infos.Get().GetResult().GetValue() );
                                wrapper.Reset();
                            } );
                    } );
            } );

        return wrapper->Promise.GetFuture();
    }

    FLinearColor GetCommitCheckColor( EGitHubToolsCommitStatusState state )
    {
        switch ( state )
        {
            case EGitHubToolsCommitStatusState::Error:
                return FLinearColor::Red;
            case EGitHubToolsCommitStatusState::Failure:
                return FLinearColor::Red;
            case EGitHubToolsCommitStatusState::Expected:
                return FLinearColor::Yellow;
            case EGitHubToolsCommitStatusState::Pending:
                return FLinearColor::Yellow;
            case EGitHubToolsCommitStatusState::Success:
                return FLinearColor::Green;
            case EGitHubToolsCommitStatusState::Unknown:
                return FLinearColor::Black;
            default:
            {
                checkNoEntry();
                return FLinearColor::Black;
            };
        }
    }

    FLinearColor GetPRChecksColor( const FGithubToolsPullRequestInfos & pr_infos )
    {
        EGitHubToolsCommitStatusState global_state = EGitHubToolsCommitStatusState::Unknown;

        for ( auto check : pr_infos.Checks )
        {
            if ( check->State < global_state )
            {
                global_state = check->State;
            }
        }

        return GetCommitCheckColor( global_state );
    }

    FString GetPullRequestReviewEventStringValue( EGitHubToolsPullRequestReviewEvent event )
    {
        switch ( event )
        {
            case EGitHubToolsPullRequestReviewEvent::Approve:
                return TEXT( "APPROVE" );
            case EGitHubToolsPullRequestReviewEvent::Comment:
                return TEXT( "COMMENT" );
            case EGitHubToolsPullRequestReviewEvent::Dismiss:
                return TEXT( "DISMISS" );
            case EGitHubToolsPullRequestReviewEvent::RequestChanges:
                return TEXT( "REQUEST_CHANGES" );
            default:
            {
                checkNoEntry();
            };
        }
        return TEXT( "" );
    }

    EGitHubToolsPullRequestReviewState GetPullRequestReviewState( const FString & event )
    {
        if ( event == TEXT( "APPROVED" ) )
        {
            return EGitHubToolsPullRequestReviewState::Approved;
        }
        if ( event == TEXT( "CHANGES_REQUESTED" ) )
        {
            return EGitHubToolsPullRequestReviewState::ChangesRequested;
        }
        if ( event == TEXT( "COMMENTED" ) )
        {
            return EGitHubToolsPullRequestReviewState::Commented;
        }
        if ( event == TEXT( "DISMISSED" ) )
        {
            return EGitHubToolsPullRequestReviewState::Dismissed;
        }
        if ( event == TEXT( "PENDING" ) )
        {
            return EGitHubToolsPullRequestReviewState::Pending;
        }
        return EGitHubToolsPullRequestReviewState::Unknown;
    }

    EGitHubToolsPullRequestsState GetPullRequestState( const FString & state )
    {
        if ( state == TEXT( "CLOSED" ) )
        {
            return EGitHubToolsPullRequestsState::Closed;
        }
        if ( state == TEXT( "MERGED" ) )
        {
            return EGitHubToolsPullRequestsState::Merged;
        }
        if ( state == TEXT( "OPEN" ) )
        {
            return EGitHubToolsPullRequestsState::Open;
        }

        return EGitHubToolsPullRequestsState::Unknown;
    }

    void MarkFileAsViewedAndExecuteCallback( const FString & pr_id, FGithubToolsPullRequestFileInfosPtr file_infos, TFunction< void( FGithubToolsPullRequestFileInfosPtr ) > callback )
    {
        if ( file_infos->ViewedState == EGitHubToolsFileViewedState::Viewed )
        {
            callback( file_infos );
            return;
        }

        FGitHubToolsModule::Get().GetNotificationManager().DisplayModalNotification( LOCTEXT( "MarkFileAsViewed", "Marking file as viewed" ) );

        FGitHubToolsModule::Get()
            .GetRequestManager()
            .SendRequest< FGitHubToolsHttpRequest_MarkFileAsViewed >( pr_id, file_infos->Path )
            .Then( [ file = MoveTemp( file_infos ), callback = MoveTemp( callback ) ]( const TFuture< FGitHubToolsHttpRequest_MarkFileAsViewed > & request ) {
                if ( request.Get().GetResult().Get( false ) )
                {
                    FGitHubToolsModule::Get().GetNotificationManager().RemoveModalNotification();

                    file->UpdateViewedState( EGitHubToolsFileViewedState::Viewed );
                    callback( file );
                }
            } );
    }

    void MarkFilesAsViewedAndExecuteCallback( FString pr_id, TArray< FGithubToolsPullRequestFileInfosPtr > && files, TFunction< void( const TArray< FGithubToolsPullRequestFileInfosPtr > & ) > && callback )
    {
        if ( files.FindByPredicate( []( const FGithubToolsPullRequestFileInfosPtr & file_infos ) {
                 return file_infos != nullptr && file_infos->ViewedState != EGitHubToolsFileViewedState::Viewed;
             } ) == nullptr )
        {
            callback( files );
            return;
        }

        FGitHubToolsModule::Get().GetNotificationManager().DisplayModalNotification( LOCTEXT( "MarkFilesAsViewed", "Marking files as viewed" ) );

        Async( EAsyncExecution::TaskGraph, [ pr_id = MoveTemp( pr_id ), callback = MoveTemp( callback ), files = MoveTemp( files ) ]() mutable {
            for ( auto file_infos : files )
            {
                if ( file_infos == nullptr )
                {
                    continue;
                }

                if ( file_infos->ViewedState == EGitHubToolsFileViewedState::Viewed )
                {
                    continue;
                }

                typedef TGitHubToolsHttpRequestWrapper< FGitHubToolsHttpRequest_MarkFileAsViewed > HttpRequestType;
                auto request = MakeShared< HttpRequestType >( pr_id, file_infos->Path );
                request->SetPromiseValueOnHttpThread();
                request->ProcessRequest();

                auto request_future_result = request->GetFuture().Get();
                if ( request_future_result.GetResult().Get( false ) )
                {
                    file_infos->UpdateViewedState( EGitHubToolsFileViewedState::Viewed );
                }
            }

            AsyncTask( ENamedThreads::GameThread, [ files = MoveTemp( files ), callback = MoveTemp( callback ) ]() {
                FGitHubToolsModule::Get().GetNotificationManager().RemoveModalNotification();

                callback( files );
            } );
        } );
    }

    void OpenAssets( const TArray< FGithubToolsPullRequestFileInfosPtr > & files )
    {
        for ( auto file : files )
        {
            if ( file == nullptr )
            {
                continue;
            }

            if ( !file->IsUAsset() )
            {
                DiffTextFile( file );
                continue;
            }

            const auto asset_data = GitHubToolsUtils::GetAssetDataFromFileInfos( *file );
            if ( asset_data.IsSet() )
            {
                const auto & asset_tools_module = FModuleManager::GetModuleChecked< FAssetToolsModule >( "AssetTools" );
                asset_tools_module.Get().OpenEditorForAssets( { asset_data.GetValue().GetAsset() } );
            }
        }
    }
}

#undef LOCTEXT_NAMESPACE