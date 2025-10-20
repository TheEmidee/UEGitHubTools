#pragma once

#include "GitHubToolsHttpRequestsTypes.h"
#include "GitHubToolsTypes.h"

class FGitHubToolsHttpRequestData_SubmitPRReview final : public FGitHubToolsHttpRequestGraphQLMutation< FString >
{
public:
    explicit FGitHubToolsHttpRequestData_SubmitPRReview( const FString & pull_request_id, const FString & pull_request_review_id, EGitHubToolsPullRequestReviewEvent event );

protected:
    FString GetRawQuery() const override;
    void ParseResponseData( const FJsonObject & json_data ) override;
    void AddParameters( FJsonObject & variables_object ) const override;

private:
    FString PullRequestId;
    FString PullRequestReviewId;
    EGitHubToolsPullRequestReviewEvent Event;
};