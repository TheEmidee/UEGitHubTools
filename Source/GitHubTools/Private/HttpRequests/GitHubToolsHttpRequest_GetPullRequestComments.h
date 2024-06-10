#pragma once

#include "GitHubToolsHttpRequestManager.h"

class FGitHubToolsHttpRequestData_GetPullRequestComments : public FGitHubToolsHttpRequestData
{
public:
    explicit FGitHubToolsHttpRequestData_GetPullRequestComments( int pull_request_number );
    EGitHubToolsRequestType GetVerb() const override;
    FString GetEndPoint() const override;
    FText GetNotificationText() const override;
    FText GetFailureText() const override;

private:
    int PullRequestNumber;
};

class FGitHubToolsHttpResponseData_GetPullRequestComments final : public FGitHubToolsHttpResponseData
{
public:
    FORCEINLINE TOptional< TArray< FGithubToolsPullRequestCommentPtr > > GetPullRequestComments() const
    {
        return Comments;
    }

    void ParseResponse( FHttpResponsePtr response_ptr ) override;

private:
    TOptional< TArray< FGithubToolsPullRequestCommentPtr > > Comments;
};