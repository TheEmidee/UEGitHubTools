#pragma once

#include "GitHubToolsHttpRequestManager.h"
#include "GitHubToolsTypes.h"

class FGitHubToolsHttpRequestData_GetPullRequestInfos final : public FGitHubToolsHttpRequest< FGithubToolsPullRequestInfosPtr >
{
public:
    typedef FGithubToolsPullRequestInfosPtr ResponseType;

    explicit FGitHubToolsHttpRequestData_GetPullRequestInfos( int pull_request_number, TArray< FGithubToolsPullRequestFileInfosPtr > files, TArray< FGithubToolsPullRequestFilePatchPtr > patches );

    FString GetBody() const override;
    void ParseResponse( FHttpResponsePtr response_ptr ) override;

private:
    int PullRequestNumber;
    TArray< FGithubToolsPullRequestFileInfosPtr > Files;
    TArray< FGithubToolsPullRequestFilePatchPtr > Patches;
};