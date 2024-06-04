#pragma once

#include "GitHubToolsTypes.h"
#include "GitSourceControlState.h"

namespace GitHubToolsGitUtils
{
    bool GetDiffNameStatusWithBranch( const FString & path_to_git_binary, const FString & repository_root, TMap< FString, FGitSourceControlState > & updated_states, TArray< FString > & error_messages, const FString & branch_name );
    TOptional< FAssetData > GetAssetDataFromFileInfos( const FGithubToolsPullRequestFileInfos & state );
    void DiffFileAgainstOriginStatusBranch( const FGithubToolsPullRequestFileInfos & file_infos );
}
