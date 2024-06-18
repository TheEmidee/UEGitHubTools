#pragma once

#include "GitHubToolsHttpRequestManager.h"
#include "GitHubToolsTypes.h"

class FGitHubToolsHttpRequestData_GetPullRequestInfos : public FGitHubToolsHttpRequest < FGithubToolsPullRequestInfosPtr >
{
public:
    typedef FGithubToolsPullRequestInfosPtr ResponseType;

    explicit FGitHubToolsHttpRequestData_GetPullRequestInfos( int pull_request_number );

    FString GetBody() const override;
    bool UsesGraphQL() const override;
    void ParseResponse( FHttpResponsePtr response_ptr ) override;

private:
    int PullRequestNumber;
};