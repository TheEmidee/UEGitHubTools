#pragma once

#include "GitSourceControlState.h"

namespace GitHubToolsGitUtils
{
    bool GetDiffNameStatusWithBranch( const FString & path_to_git_binary, const FString & repository_root, TMap< FString, FGitSourceControlState > & updated_states, TArray< FString > & error_messages, const FString & branch_name );
    TOptional< FAssetData > GetAssetDataFromState( const TSharedRef< FGitSourceControlState > & state );
    void DiffAssetAgainstOriginStatusBranch( const TSharedRef< FGitSourceControlState > & state );
}
