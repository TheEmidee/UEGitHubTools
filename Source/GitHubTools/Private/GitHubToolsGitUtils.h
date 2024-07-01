#pragma once

#include "GitHubToolsTypes.h"

namespace GitHubToolsUtils
{
    TOptional< FAssetData > GetAssetDataFromFileInfos( const FGithubToolsPullRequestFileInfos & file_infos );
    void DiffFileAgainstOriginStatusBranch( const FGithubToolsPullRequestFileInfos & file_infos );

    TFuture< FGithubToolsPullRequestInfosPtr > GetPullRequestInfos( int pr_number );

    FLinearColor GetCommitCheckColor( EGitHubToolsCommitStatusState state );
    FLinearColor GetPRChecksColor( const FGithubToolsPullRequestInfos & pr_infos );
    FString GetPullRequestReviewEventStringValue( EGitHubToolsPullRequestReviewEvent event );
    EGitHubToolsPullRequestReviewState GetPullRequestReviewState( const FString & state );
    EGitHubToolsPullRequestsState GetPullRequestState( const FString & state );
}
