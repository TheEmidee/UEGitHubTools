#include "GitHubToolsGitUtils.h"

#include "GitHubTools.h"
#include "GitSourceControlModule.h"
#include "GitSourceControlUtils.h"
#include "HttpRequests/GitHubToolsHttpRequest_GetPullRequestFilePatches.h"
#include "HttpRequests/GitHubToolsHttpRequest_GetPullRequestInfos.h"
#include "HttpRequests/GitHubToolsHttpRequest_MarkFileAsViewed.h"
#include "Widgets/SGitHubToolsFilePatch.h"

#include "AssetDefinition.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "FileHelpers.h"
#include "IHotReload.h"
#include "ILiveCodingModule.h"
#include "ISourceControlModule.h"
#include "SourceControlOperations.h"
#include "Async/Async.h"
#include "Framework/Application/SlateApplication.h"
#include "HttpRequests/GitHubToolsHttpRequest_GetPullRequestFiles.h"
#include "Misc/Optional.h"

#define LOCTEXT_NAMESPACE "GitHubTools.Requests"

namespace GitHubToolsUtils
{
    FString GetBranchName()
    {
        auto & git_source_control = FGitSourceControlModule::Get();
        auto & provider = git_source_control.GetProvider();
        const auto & path_to_repository_root = provider.GetPathToRepositoryRoot();
        const auto & path_to_git_binary = git_source_control.AccessSettings().GetBinaryPath();
	
        TArray<FString> InfoMessages;
        TArray<FString> ErrorMessages;
        TArray<FString> Parameters;
        Parameters.Add(TEXT("--short"));
        Parameters.Add(TEXT("--quiet")); // no error message while in detached HEAD
        Parameters.Add(TEXT("HEAD"));
        if (GitSourceControlUtils::RunCommand(TEXT("symbolic-ref"), path_to_git_binary, path_to_repository_root, Parameters, FGitSourceControlModule::GetEmptyStringArray(), InfoMessages, ErrorMessages) && InfoMessages.Num() > 0)
        {
            return InfoMessages[0];
        }

        Parameters.Reset(2);
        Parameters.Add(TEXT("-1"));
        Parameters.Add(TEXT("--format=\"%h\"")); // no error message while in detached HEAD
        if (GitSourceControlUtils::RunCommand(TEXT("log"), path_to_git_binary, path_to_repository_root, Parameters, FGitSourceControlModule::GetEmptyStringArray(), InfoMessages, ErrorMessages) && InfoMessages.Num() > 0)
        {
            return "HEAD detached at " + InfoMessages[0];
        }

        return "";
    }

    TOptional< FAssetData > GetAssetDataFromFileInfos( const FGithubToolsPullRequestFileInfos & file_infos )
    {
        const auto & git_source_control = FModuleManager::GetModuleChecked< FGitSourceControlModule >( "GitSourceControl" );
        const auto & path_to_repository_root = git_source_control.GetProvider().GetPathToRepositoryRoot();

        const auto absolute_path = FPaths::ConvertRelativePathToFull( path_to_repository_root, file_infos.Path );

        if (FString package_name;
            FPackageName::TryConvertFilenameToLongPackageName( absolute_path, package_name ))
        {
            TArray< FAssetData > assets;
            FModuleManager::LoadModuleChecked< FAssetRegistryModule >( TEXT( "AssetRegistry" ) ).Get().GetAssetsByPackageName( *package_name, assets );
            if (assets.Num() == 1)
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

        if (const TSharedPtr< SWindow > active_modal = FSlateApplication::Get().GetActiveModalWindow();
            active_modal.IsValid())
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
        if (!optional_asset_data.IsSet())
        {
            return;
        }

        const auto & git_source_control = FModuleManager::GetModuleChecked< FGitSourceControlModule >( "GitSourceControl" );
        const auto & path_to_repository_root = git_source_control.GetProvider().GetPathToRepositoryRoot();
        const auto & status_branch_names = FGitSourceControlModule::Get().GetProvider().GetStatusBranchNames();
        if (status_branch_names.IsEmpty())
        {
            return;
        }

        const auto & branch_name = status_branch_names[ 0 ];
        const auto & path_to_git_binary = git_source_control.AccessSettings().GetBinaryPath();

        auto asset_data = optional_asset_data.GetValue();

        const auto package_path = asset_data.PackageName.ToString();
        const auto package_name = asset_data.AssetName.ToString();
        auto * current_object = asset_data.GetAsset();

        // Get the file name of package
        FString relative_file_name;
        if (!FPackageName::DoesPackageExist( package_path, &relative_file_name ))
        {
            return;
        }

        TArray< FString > errors;
        const auto & revision = GitSourceControlUtils::GetOriginRevisionOnBranch( path_to_git_binary, path_to_repository_root, relative_file_name, errors, branch_name );

        check( revision.IsValid() );

        FString temp_file_name;
        if (!revision->Get( temp_file_name ))
        {
            return;
        }

        auto * temp_package = LoadPackage( nullptr, *temp_file_name, LOAD_ForDiff | LOAD_DisableCompileOnLoad );
        if (temp_package == nullptr)
        {
            return;
        }

        auto * old_object = FindObject< UObject >( temp_package, *package_name );
        if (old_object == nullptr)
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
        if (file_infos == nullptr)
        {
            return;
        }

        if (!file_infos->IsUAsset())
        {
            DiffTextFile( file_infos );
        }
        else
        {
            DiffUAsset( *file_infos );
        }
    }

    void DiffFilesAgainstOriginStatusBranch( const TArray< FGithubToolsPullRequestFileInfosPtr > & file_infos )
    {
        for (auto file : file_infos)
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
        switch (state)
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

        for (auto check : pr_infos.Checks)
        {
            if (check->State < global_state)
            {
                global_state = check->State;
            }
        }

        return GetCommitCheckColor( global_state );
    }

    FString GetPullRequestReviewEventStringValue( EGitHubToolsPullRequestReviewEvent event )
    {
        switch (event)
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
        if (event == TEXT( "APPROVED" ))
        {
            return EGitHubToolsPullRequestReviewState::Approved;
        }
        if (event == TEXT( "CHANGES_REQUESTED" ))
        {
            return EGitHubToolsPullRequestReviewState::ChangesRequested;
        }
        if (event == TEXT( "COMMENTED" ))
        {
            return EGitHubToolsPullRequestReviewState::Commented;
        }
        if (event == TEXT( "DISMISSED" ))
        {
            return EGitHubToolsPullRequestReviewState::Dismissed;
        }
        if (event == TEXT( "PENDING" ))
        {
            return EGitHubToolsPullRequestReviewState::Pending;
        }
        return EGitHubToolsPullRequestReviewState::Unknown;
    }

    EGitHubToolsPullRequestsState GetPullRequestState( const FString & state )
    {
        if (state == TEXT( "CLOSED" ))
        {
            return EGitHubToolsPullRequestsState::Closed;
        }
        if (state == TEXT( "MERGED" ))
        {
            return EGitHubToolsPullRequestsState::Merged;
        }
        if (state == TEXT( "OPEN" ))
        {
            return EGitHubToolsPullRequestsState::Open;
        }

        return EGitHubToolsPullRequestsState::Unknown;
    }

    void MarkFileAsViewedAndExecuteCallback( const FString & pr_id, FGithubToolsPullRequestFileInfosPtr file_infos, TFunction< void( FGithubToolsPullRequestFileInfosPtr ) > callback )
    {
        if (file_infos->ViewedState == EGitHubToolsFileViewedState::Viewed)
        {
            callback( file_infos );
            return;
        }

        FGitHubToolsModule::Get()
            .GetRequestManager()
            .SendRequest< FGitHubToolsHttpRequest_MarkFileAsViewed >( pr_id, file_infos->Path )
            .Then( [ file = MoveTemp( file_infos ), callback = MoveTemp( callback ) ]( const TFuture< FGitHubToolsHttpRequest_MarkFileAsViewed > & request ) {
                if (request.Get().GetResult().Get( false ))
                {
                    FGitHubToolsModule::Get().GetNotificationManager().RemoveModalNotification();

                    file->UpdateViewedState( EGitHubToolsFileViewedState::Viewed );
                    callback( file );
                }
            } );
    }

    void MarkFilesAsViewedAndExecuteCallback( FString pr_id, TArray< FGithubToolsPullRequestFileInfosPtr > && files, TFunction< void( const TArray< FGithubToolsPullRequestFileInfosPtr > & ) > && callback )
    {
        if (files.FindByPredicate( []( const FGithubToolsPullRequestFileInfosPtr & file_infos ) {
            return file_infos != nullptr && file_infos->ViewedState != EGitHubToolsFileViewedState::Viewed;
        } ) == nullptr)
        {
            callback( files );
            return;
        }

        Async( EAsyncExecution::TaskGraph,
            [ pr_id = MoveTemp( pr_id ), callback = MoveTemp( callback ), files = MoveTemp( files ) ]() mutable {
                for (auto file_infos : files)
                {
                    if (file_infos == nullptr)
                    {
                        continue;
                    }

                    if (file_infos->ViewedState == EGitHubToolsFileViewedState::Viewed)
                    {
                        continue;
                    }

                    typedef TGitHubToolsHttpRequestWrapper< FGitHubToolsHttpRequest_MarkFileAsViewed > HttpRequestType;
                    auto request = MakeShared< HttpRequestType >( pr_id, file_infos->Path );
                    request->SetPromiseValueOnHttpThread();
                    request->ProcessRequest();

                    auto request_future_result = request->GetFuture().Get();
                    if (request_future_result.GetResult().Get( false ))
                    {
                        file_infos->UpdateViewedState( EGitHubToolsFileViewedState::Viewed );
                    }
                }

                AsyncTask( ENamedThreads::GameThread,
                    [ files = MoveTemp( files ), callback = MoveTemp( callback ) ]() {
                        FGitHubToolsModule::Get().GetNotificationManager().RemoveModalNotification();

                        callback( files );
                    } );
            } );
    }

    void OpenAssets( const TArray< FGithubToolsPullRequestFileInfosPtr > & files )
    {
        for (auto file : files)
        {
            if (file == nullptr)
            {
                continue;
            }

            if (!file->IsUAsset())
            {
                DiffTextFile( file );
                continue;
            }

            const auto asset_data = GitHubToolsUtils::GetAssetDataFromFileInfos( *file );
            if (asset_data.IsSet())
            {
                const auto & asset_tools_module = FModuleManager::GetModuleChecked< FAssetToolsModule >( "AssetTools" );
                asset_tools_module.Get().OpenEditorForAssets( { asset_data.GetValue().GetAsset() } );
            }
        }
    }

    void GetModifiedFiles( TArray< FString > & package_names )
    {
        TArray< UPackage * > LoadedPackages;
        TMap< FString, FSourceControlStatePtr > PackageStates;
        FEditorFileUtils::FindAllSubmittablePackageFiles( PackageStates, true );

        for (TMap< FString, FSourceControlStatePtr >::TConstIterator PackageIter( PackageStates ); PackageIter; ++PackageIter)
        {
            const FString PackageName = *PackageIter.Key();

            UPackage * Package = FindPackage( nullptr, *PackageName );
            if (Package != nullptr)
            {
                LoadedPackages.Add( Package );
            }

            package_names.Add( PackageName );
        }

        // Get a list of all the checked out project files
        TMap< FString, FSourceControlStatePtr > ProjectFileStates;
        FEditorFileUtils::FindAllSubmittableProjectFiles( ProjectFileStates );
        for (TMap< FString, FSourceControlStatePtr >::TConstIterator It( ProjectFileStates ); It; ++It)
        {
            package_names.Add( It.Key() );
        }

        // Get a list of all the checked out config files
        TMap< FString, FSourceControlStatePtr > ConfigFileStates;
        FEditorFileUtils::FindAllSubmittableConfigFiles( ConfigFileStates );
        for (TMap< FString, FSourceControlStatePtr >::TConstIterator It( ConfigFileStates ); It; ++It)
        {
            package_names.Add( It.Key() );
        }
    }

    bool RevertFiles()
    {
        TArray< FString > modified_files;
        GetModifiedFiles( modified_files );

        if ( !modified_files.IsEmpty() )
        {
            auto & source_control_module = ISourceControlModule::Get();
            ISourceControlProvider & source_control_provider = source_control_module.GetProvider();
            TSharedRef< FRevert, ESPMode::ThreadSafe > RevertOperation = ISourceControlOperation::Create< FRevert >();
            if ( source_control_provider.Execute( RevertOperation, modified_files ) != ECommandResult::Succeeded )
            {
                return false;
            }
        }

        return true;
    }

    bool SwitchGitBranch( const FString & branch_name )
    {
        // Switch the git branch
        {
            auto & git_source_control = FGitSourceControlModule::Get();
            auto & provider = git_source_control.GetProvider();
            const auto & path_to_repository_root = provider.GetPathToRepositoryRoot();
            const auto & path_to_git_binary = git_source_control.AccessSettings().GetBinaryPath();

            TArray< FString > Results;
            TArray< FString > Parameters = { branch_name, "-f" };
            TArray< FString > Files;
            TArray< FString > OutErrorMessages;
            if (!GitSourceControlUtils::RunCommand( TEXT( "switch" ), path_to_git_binary, path_to_repository_root, Parameters, Files, Results, OutErrorMessages ))
            {
                FGitHubToolsModule::Get().GetNotificationManager().DisplayFailureNotification( LOCTEXT( "CantSwitchBranch", "Impossible to switch to the new branch" ) );
                return false;
            }

            Parameters = { "--init", "--recursive" };
            
            if (!GitSourceControlUtils::RunCommand( TEXT( "submodule update" ), path_to_git_binary, path_to_repository_root, Parameters, Files, Results, OutErrorMessages ))
            {
                FGitHubToolsModule::Get().GetNotificationManager().DisplayFailureNotification( LOCTEXT( "CantSwitchBranch", "Impossible to update the submodules" ) );
                return false;
            }
        }

        // Recompile the code
        {
            const auto compile = []()
                {
#if WITH_LIVE_CODING
                    ILiveCodingModule * LiveCoding = FModuleManager::GetModulePtr< ILiveCodingModule >( LIVE_CODING_MODULE_NAME );
                    if (LiveCoding != nullptr && LiveCoding->IsEnabledByDefault())
                    {
                        LiveCoding->EnableForSession( true );
                        if (LiveCoding->IsEnabledForSession())
                        {
                            return LiveCoding->Compile(ELiveCodingCompileFlags::WaitForCompletion, nullptr);
                        }
                    }
                    else
#endif
                    {
#if WITH_HOT_RELOAD
                        IHotReloadInterface * HotReload = IHotReloadModule::GetPtr();
                        if (HotReload != nullptr && !HotReload->IsCurrentlyCompiling())
                        {
                            const auto result = HotReload->DoHotReloadFromEditor( EHotReloadFlags::None );
                            return result == ECompilationResult::Succeeded || result == ECompilationResult::UpToDate;
                        }
#endif
                    }
                    return true;
                };

            if (!compile())
            {
                FGitHubToolsModule::Get().GetNotificationManager().DisplayFailureNotification( LOCTEXT( "CantSwitchBranch", "Impossible to compile the code" ) );
                return false;
            }
        }

        // Rescan for new assets
        {
            FAssetRegistryModule & AssetRegistryModule = FModuleManager::LoadModuleChecked< FAssetRegistryModule >( "AssetRegistry" );
            IAssetRegistry & AssetRegistry = AssetRegistryModule.Get();

            // Clear existing data if you want a full refresh
            AssetRegistry.ScanModifiedAssetFiles( TArray< FString >() ); // Clears modified cache

            // Force a full rescan of the content directory
            TArray< FString > PathsToScan;
            PathsToScan.Add( FPaths::ProjectContentDir() ); // Or additional directories if needed

            UE_LOG( LogTemp, Log, TEXT("Rescanning all asset files...") );
            AssetRegistry.ScanPathsSynchronous( PathsToScan, true ); // true = force rescan
            UE_LOG( LogTemp, Log, TEXT("Rescan complete.") );
        }

        return true;
    }
}

#undef LOCTEXT_NAMESPACE