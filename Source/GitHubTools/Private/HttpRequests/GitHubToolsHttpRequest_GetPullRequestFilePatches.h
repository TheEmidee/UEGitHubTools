#pragma once

#include "GitHubToolsHttpRequestManager.h"
#include "GitHubToolsTypes.h"

class FGitHubToolsHttpRequestData_GetPullRequestFilePatches final : public FGitHubToolsHttpRequest< TArray< FGithubToolsPullRequestFileInfosPtr > >
{
public:
    typedef TArray< FGithubToolsPullRequestFileInfosPtr > ResponseType;

    explicit FGitHubToolsHttpRequestData_GetPullRequestFilePatches( int pull_request_number, const FString & after_cursor = TEXT( "" ) );

    FString GetEndPoint() const override;
    bool UsesGraphQL() const override;
    void ParseResponse( FHttpResponsePtr response_ptr ) override;

private:
    int PullRequestNumber;
};