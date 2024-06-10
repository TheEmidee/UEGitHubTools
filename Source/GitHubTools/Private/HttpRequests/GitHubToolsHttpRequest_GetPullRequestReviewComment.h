#pragma once

#include "GitHubToolsHttpRequestManager.h"

class FGitHubToolsHttpRequestData_GetPullRequestReviewComments : public FGitHubToolsHttpRequestData
{
public:
    explicit FGitHubToolsHttpRequestData_GetPullRequestReviewComments( int pull_request_number, int review_id );
    EGitHubToolsRequestType GetVerb() const override;
    FString GetEndPoint() const override;
    FText GetNotificationText() const override;
    FText GetFailureText() const override;

private:
    int PullRequestNumber;
    int ReviewId;
};

class FGitHubToolsHttpResponseData_GetPullRequestReviewComments final : public FGitHubToolsHttpResponseData
{
public:
    FORCEINLINE TOptional< TArray< FGithubToolsPullRequestCommentPtr > > GetPullRequestReviewComments() const
    {
        return ReviewComments;
    }

    void ParseResponse( FHttpResponsePtr response_ptr ) override;

private:
    TOptional< TArray< FGithubToolsPullRequestCommentPtr > > ReviewComments;
};