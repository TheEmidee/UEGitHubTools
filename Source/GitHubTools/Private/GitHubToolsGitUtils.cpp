#include "GitHubToolsGitUtils.h"

#include "GitHubTools.h"
#include "GitSourceControlModule.h"
#include "GitSourceControlUtils.h"
#include "HttpRequests/GitHubToolsHttpRequest_GetPullRequestInfos.h"

#include <AssetDefinition.h>
#include <AssetRegistry/AssetRegistryModule.h>
#include <AssetToolsModule.h>
#include <HttpRequests/GitHubToolsHttpRequest_GetPullRequestFiles.h>

#define LOCTEXT_NAMESPACE "GitHubTools.Requests"

namespace GitHubToolsUtils
{
    bool GetDiffNameStatusWithBranch( const FString & path_to_git_binary, const FString & repository_root, TMap< FString, FGitSourceControlState > & updated_states, TArray< FString > & error_messages, const FString & branch_name )
    {
        TArray< FString > results;
        if ( !GitSourceControlUtils::RunCommand( TEXT( "diff" ),
                 path_to_git_binary,
                 repository_root,
                 { FString::Printf( TEXT( "%s..." ), *branch_name ),
                     TEXT( "--name-status" ) },
                 FGitSourceControlModule::GetEmptyStringArray(),
                 results,
                 error_messages ) )
        {
            return false;
        }

        TArray< FString > files;
        TMap< FString, FString > results_map;

        for ( const auto & result : results )
        {
            TArray< FString > split;
            result.ParseIntoArray( split, TEXT( "\t" ) );

            const FString & relative_filename = split[ 1 ];
            const FString & file = FPaths::ConvertRelativePathToFull( repository_root, relative_filename );
            results_map.Add( file, result );

            files.Emplace( file );
        }

        GitSourceControlUtils::ParseStatusResults( path_to_git_binary, repository_root, false, files, results_map, updated_states );

        // ParseStatusResults keeps TreeState to ETreeState::UnSet, which prevents the diff tool to work
        for ( auto & [ file_name, state ] : updated_states )
        {
            state.State.TreeState = ETreeState::Unmodified;
        }

        return true;
    }

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

    void DiffFileAgainstOriginStatusBranch( const FGithubToolsPullRequestFileInfos & file_infos )
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
                    .SendRequest< FGitHubToolsHttpRequestData_GetPullRequestInfos >( pr_number )
                    .Then( [ &, files = MoveTemp( files ) ]( const TFuture< FGitHubToolsHttpRequestData_GetPullRequestInfos > & get_pr_infos ) {
                        const auto optional_result = get_pr_infos.Get().GetResult();

                        auto pr_infos = optional_result.GetValue();

                        if ( optional_result.IsSet() )
                        {
                            pr_infos->FileInfos.Append( files );
                        }

                        wrapper->Promise.SetValue( pr_infos );
                        wrapper.Reset();
                    } );
            } );

        return wrapper->Promise.GetFuture();
    }
}

#undef LOCTEXT_NAMESPACE