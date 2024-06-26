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

    FLinearColor GetCommitCheckColor( EGitHubToolsCommitStatusState state );
    FLinearColor GetPRChecksColor( const FGithubToolsPullRequestInfos & pr_infos );
}
