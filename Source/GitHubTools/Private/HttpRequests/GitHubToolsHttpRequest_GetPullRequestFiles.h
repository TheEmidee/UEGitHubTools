#pragma once

#include "GitHubToolsHttpRequestManager.h"
#include "GitHubToolsTypes.h"

class FGitHubToolsHttpRequestData_GetPullRequestFiles : public FGitHubToolsHttpRequest< TArray< FGithubToolsPullRequestFileInfosPtr > >
{
public:
    typedef TArray< FGithubToolsPullRequestFileInfosPtr > ResponseType;

    explicit FGitHubToolsHttpRequestData_GetPullRequestFiles( int pull_request_number, const FString & after_cursor = TEXT( "" ) );

    FString GetBody() const override;
    bool UsesGraphQL() const override;
    void ParseResponse( FHttpResponsePtr response_ptr ) override;

    bool HasNextPage() const
    {
        return bHasNextPage;
    }

    FString GetEndCursor() const
    {
        return EndCursor;
    }

private:
    int PullRequestNumber;
    FString AfterCursor;
    bool bHasNextPage;
    FString EndCursor;
};