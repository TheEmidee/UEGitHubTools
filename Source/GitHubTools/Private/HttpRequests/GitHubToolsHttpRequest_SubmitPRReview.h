#pragma once

#include "GitHubToolsHttpRequestManager.h"
#include "GitHubToolsTypes.h"

class FGitHubToolsHttpRequestData_SubmitPRReview : public FGitHubToolsHttpRequest< FString >
{
public:
    explicit FGitHubToolsHttpRequestData_SubmitPRReview( const FString & pull_request_id, const FString & pull_request_review_id, EGitHubToolsPullRequestReviewEvent event );

    FString GetBody() const override;
    bool UsesGraphQL() const override;

private:
    void ParseResponse( FHttpResponsePtr response_ptr ) override;

    FString PullRequestId;
    FString PullRequestReviewId;
    EGitHubToolsPullRequestReviewEvent Event;
};