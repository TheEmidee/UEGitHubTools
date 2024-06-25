#pragma once

#include "GitHubTools.h"
#include "GitHubToolsTypes.h"
#include "GitSourceControlState.h"

namespace GitHubToolsUtils
{
    bool GetDiffNameStatusWithBranch( const FString & path_to_git_binary, const FString & repository_root, TMap< FString, FGitSourceControlState > & updated_states, TArray< FString > & error_messages, const FString & branch_name );
    TOptional< FAssetData > GetAssetDataFromFileInfos( const FGithubToolsPullRequestFileInfos & state );
    void DiffFileAgainstOriginStatusBranch( const FGithubToolsPullRequestFileInfos & file_infos );

    TFuture< FGithubToolsPullRequestInfosPtr > GetPullRequestInfos( int pr_number );

    //template < typename TRequestType, typename... TArgTypes >
    //TFuture< typename TRequestType::ResultType > RunPaginatedRequest( TArgTypes &&... args )
    //{
    //    return Async( EAsyncExecution::TaskGraph, [ ... args = MoveTemp( args ) ]() {
    //        typedef typename TRequestType::ResultType TResultType;

    //        FString cursor;
    //        //TPromise< TResultType > promise;
    //        TResultType result;

    //        while ( true )
    //        {
    //            const auto request = FGitHubToolsModule::Get()
    //                                     .GetRequestManager()
    //                                     .SendRequest< TRequestType >( args ..., cursor )
    //                                     .Get();

    //            const auto optional_result = request.GetResult();

    //            if ( !optional_result.IsSet() )
    //            {
    //                //promise.SetValue( result );
    //                break;
    //            }

    //            result.Append( optional_result.GetValue() );

    //            if ( !request.HasNextPage() )
    //            {
    //                //promise.SetValue( result );
    //                break;
    //            }

    //            cursor = request.GetEndCursor();
    //        }

    //        return result;
    //    } );
    //}
}
