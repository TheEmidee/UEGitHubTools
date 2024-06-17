#include "GitHubToolsGitUtils.h"

#include "GitHubTools.h"
#include "GitSourceControlModule.h"
#include "GitSourceControlUtils.h"

#include <AssetDefinition.h>
#include <AssetRegistry/AssetRegistryModule.h>
#include <AssetToolsModule.h>

namespace GitHubToolsGitUtils
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

    //TFuture< FGithubToolsPullRequestInfosPtr > GetPullRequestInfos()
    //{
    //    //FGithubToolsPullRequestInfosPtr pr_infos = MakeShared< FGithubToolsPullRequestInfos >();

    //    auto result =
    //        FGitHubToolsModule::Get()
    //            .GetRequestManager()
    //            .SendRequest< FGitHubToolsHttpRequestData_GetPullRequestNumber, FGitHubToolsHttpResponseData_GetPullRequestNumber >()
    //            .Then( [ & ]( TFuture< FGitHubToolsHttpResponseData_GetPullRequestNumber > result ) {
    //                const auto pr_number = 573; // response_data.GetPullRequestNumber().Get( INDEX_NONE );
    //                /*if ( pr_number == INDEX_NONE )
    //                {
    //                    return;
    //                }*/

    //                return FGitHubToolsModule::Get()
    //                    .GetRequestManager()
    //                    .SendRequest< FGitHubToolsHttpRequestData_GetPullRequestFiles, FGitHubToolsHttpResponseData_GetPullRequestFiles >( result.Get() );
    //            } )
    //            .Then( []( TFuture< FGitHubToolsHttpResponseData_GetPullRequestFiles > result ) {
    //                return FGitHubToolsModule::Get()
    //                    .GetRequestManager()
    //                    .SendRequest< FGitHubToolsHttpRequestData_GetPullRequestComments, FGitHubToolsHttpResponseData_GetPullRequestComments >( 573 );
    //            } )
    //            .Then( []( TFuture < FGitHubToolsHttpResponseData_GetPullRequestComments  > result ) {
    //                return MakeFulfilledPromise< FGithubToolsPullRequestInfosPtr >().GetFuture();
    //            } );

    //    return result;

    //    //        auto pr_infos = MakeShared< FGithubToolsPullRequestInfos >();

    //    //

    //    //                pr_infos->FileInfos = optional_files.GetValue();

    //    //                FGitHubToolsModule::Get()
    //    //                    .GetRequestManager()
    //    //                    .SendRequest< FGitHubToolsHttpRequestData_GetPullRequestComments, FGitHubToolsHttpResponseData_GetPullRequestComments >( pr_number )
    //    //                    .Next( [ &, pr_infos, pr_number ]( const FGitHubToolsHttpResponseData_GetPullRequestComments & get_comments_data ) {
    //    //                        const auto optional_comments = get_comments_data.GetPullRequestComments();
    //    //                        if ( optional_comments.IsSet() )
    //    //                        {
    //    //                            pr_infos->Comments = optional_comments.GetValue();
    //    //                        }

    //    //                        FGitHubToolsModule::Get()
    //    //                            .GetRequestManager()
    //    //                            .SendRequest< FGitHubToolsHttpRequestData_GetPullRequestReviews, FGitHubToolsHttpResponseData_GetPullRequestReviews >( pr_number )
    //    //                            .Then( [ &, pr_infos, pr_number ]( TFuture< FGitHubToolsHttpResponseData_GetPullRequestReviews > get_reviews_data ) {

    //    //                                const auto optional_reviews = get_reviews_data.Get().GetPullRequestReviews();
    //    //                                if ( !optional_reviews.GetValue().IsEmpty() )
    //    //                                {

    //    //                                    pr_infos->Reviews = optional_reviews.GetValue();

    //    //                                    for ( auto review : pr_infos->Reviews )
    //    //                                    {
    //    //                                        const auto comments_data = FGitHubToolsModule::Get()
    //    //                                                                       .GetRequestManager()
    //    //                                                                       .SendRequest< FGitHubToolsHttpRequestData_GetPullRequestReviewComments, FGitHubToolsHttpResponseData_GetPullRequestReviewComments >( pr_number, review->Id )
    //    //                                                                       .Get();

    //    //                                        const auto optional_review_comments = comments_data.GetPullRequestReviewComments();
    //    //                                        if ( optional_review_comments.IsSet() )
    //    //                                        {
    //    //                                            review->Comments = optional_review_comments.GetValue();
    //    //                                        }
    //    //                                    }
    //    //                                }

    //    //                                callback( pr_infos );
    //    //                            } );
    //    //                    } );
    //    //            } );
    //    //    } );

    //    ////return MakeFulfilledPromise< FGithubToolsPullRequestInfosPtr >( pr_infos ).GetFuture();
    //}
}