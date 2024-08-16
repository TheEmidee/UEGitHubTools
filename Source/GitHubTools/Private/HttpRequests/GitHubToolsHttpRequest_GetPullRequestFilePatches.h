#pragma once

#include "GitHubToolsHttpRequestManager.h"
#include "GitHubToolsTypes.h"

class FGitHubToolsHttpRequestData_GetPullRequestFilePatches final : public FGitHubToolsHttpRequest< TArray< FGithubToolsPullRequestFilePatchPtr > >
{
public:
    typedef TArray< bool > ResponseType;

    explicit FGitHubToolsHttpRequestData_GetPullRequestFilePatches( int pull_request_number );

    FString GetEndPoint() const override;
    bool UsesGraphQL() const override;
    void ParseResponse( FHttpResponsePtr response_ptr ) override;

private:
    int PullRequestNumber;
};