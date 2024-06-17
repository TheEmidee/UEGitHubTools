#pragma once

#include "GitHubToolsHttpRequestManager.h"
#include "GitHubToolsTypes.h"

class FGitHubToolsHttpRequestData_SubmitPRReview : public FGitHubToolsHttpRequestData
{
public:
    explicit FGitHubToolsHttpRequestData_SubmitPRReview( const FString & pull_request_id, const FString & pull_request_review_id, EGitHubToolsPullRequestReviewEvent event );

    FString GetBody() const override;
    bool UsesGraphQL() const override;
    FText GetNotificationText() const override;
    FText GetFailureText() const override;

private:
    FString PullRequestId;
    FString PullRequestReviewId;
    EGitHubToolsPullRequestReviewEvent Event;
};

class FGitHubToolsHttpResponseData_SubmitPRReview final : public FGitHubToolsHttpResponseData
{
public:
    FORCEINLINE TOptional< FString > GetPullRequestReviewId() const
    {
        return PullRequestReviewId;
    }

    void ParseResponse( FHttpResponsePtr response_ptr ) override;

private:
    TOptional< FString > PullRequestReviewId;
};