#pragma once

#include "GitHubToolsTypes.h"

namespace GitHubToolsUtils
{
    TOptional< FAssetData > GetAssetDataFromFileInfos( const FGithubToolsPullRequestFileInfos & file_infos );
    void DiffFileAgainstOriginStatusBranch( const FGithubToolsPullRequestFileInfosPtr & file_infos );
    void DiffFilesAgainstOriginStatusBranch( const TArray< FGithubToolsPullRequestFileInfosPtr > & file_infos );
    TFuture< FGithubToolsPullRequestInfosPtr > GetPullRequestInfos( int pr_number );
    FLinearColor GetCommitCheckColor( EGitHubToolsCommitStatusState state );
    FLinearColor GetPRChecksColor( const FGithubToolsPullRequestInfos & pr_infos );
    FString GetPullRequestReviewEventStringValue( EGitHubToolsPullRequestReviewEvent event );
    EGitHubToolsPullRequestReviewState GetPullRequestReviewState( const FString & state );
    EGitHubToolsPullRequestsState GetPullRequestState( const FString & state );
    void MarkFileAsViewedAndExecuteCallback( const FString & pr_id, FGithubToolsPullRequestFileInfosPtr file_infos, TFunction< void( FGithubToolsPullRequestFileInfosPtr ) > callback );
    void MarkFilesAsViewedAndExecuteCallback( ::FString pr_id, TArray< FGithubToolsPullRequestFileInfosPtr > && files, TFunction< void( const TArray< FGithubToolsPullRequestFileInfosPtr > & ) > && callback );
    void OpenAssets( const TArray< FGithubToolsPullRequestFileInfosPtr > & files );
}
