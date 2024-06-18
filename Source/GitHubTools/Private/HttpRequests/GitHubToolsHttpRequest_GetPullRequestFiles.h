#pragma once

#include "GitHubToolsHttpRequestManager.h"
#include "GitHubToolsTypes.h"

class FGitHubToolsHttpRequestData_GetPullRequestFiles : public FGitHubToolsHttpRequestWithPagination< TArray< FGithubToolsPullRequestFileInfosPtr > >
{
public:
    typedef TArray< FGithubToolsPullRequestFileInfosPtr > ResponseType;

    explicit FGitHubToolsHttpRequestData_GetPullRequestFiles( int pull_request_number, const FString & after_cursor = TEXT( "" ) );

    FString GetBody() const override;
    bool UsesGraphQL() const override;
    void ParseResponse( FHttpResponsePtr response_ptr ) override;

private:
    int PullRequestNumber;
};