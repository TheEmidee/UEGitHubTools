#pragma once

#include "GitHubToolsHttpRequestManager.h"
#include "GitHubToolsTypes.h"

class FGitHubToolsHttpRequestData_GetPullRequestFilePatches final : public FGitHubToolsHttpRequestQueryRest< TArray< FGithubToolsPullRequestFilePatchPtr > >
{
public:
    typedef TArray< bool > ResponseType;

    explicit FGitHubToolsHttpRequestData_GetPullRequestFilePatches( int pull_request_number );

    FString GetEndPoint() const override;
    void ParseResponse( FHttpResponsePtr response_ptr ) override;

protected:
    FString GetRawQuery() const override;

private:
    int PullRequestNumber;
};