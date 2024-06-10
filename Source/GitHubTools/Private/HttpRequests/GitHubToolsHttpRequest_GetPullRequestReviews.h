#pragma once

#include "GitHubToolsHttpRequestManager.h"

class FGitHubToolsHttpRequestData_GetPullRequestReviews : public FGitHubToolsHttpRequestData
{
public:
    explicit FGitHubToolsHttpRequestData_GetPullRequestReviews( int pull_request_number );
    EGitHubToolsRequestType GetVerb() const override;
    FString GetEndPoint() const override;
    FText GetNotificationText() const override;
    FText GetFailureText() const override;

private:
    int PullRequestNumber;
};

class FGitHubToolsHttpResponseData_GetPullRequestReviews final : public FGitHubToolsHttpResponseData
{
public:
    FORCEINLINE TOptional< TArray< FGithubToolsPullRequestReviewInfosPtr > > GetPullRequestReviews() const
    {
        return Reviews;
    }

    void ParseResponse( FHttpResponsePtr response_ptr ) override;

private:
    TOptional< TArray< FGithubToolsPullRequestReviewInfosPtr > > Reviews;
};