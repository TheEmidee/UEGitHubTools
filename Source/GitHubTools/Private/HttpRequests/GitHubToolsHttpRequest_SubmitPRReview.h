#pragma once

#include "GitHubToolsHttpRequestManager.h"
#include "GitHubToolsTypes.h"

class FGitHubToolsHttpRequestData_SubmitPRReview final : public FGitHubToolsHttpRequestGraphQLMutation< FString >
{
public:
    explicit FGitHubToolsHttpRequestData_SubmitPRReview( const FString & pull_request_id, const FString & pull_request_review_id, EGitHubToolsPullRequestReviewEvent event );

    FString GetRawQuery() const override;

private:
    void ParseResponseData( const FJsonObject & json_data ) override;

    FString PullRequestId;
    FString PullRequestReviewId;
    EGitHubToolsPullRequestReviewEvent Event;
};