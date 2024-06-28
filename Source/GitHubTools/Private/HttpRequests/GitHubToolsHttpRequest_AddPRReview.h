#pragma once

#include "GitHubToolsHttpRequestManager.h"
#include "GitHubToolsTypes.h"

class FGitHubToolsHttpRequestData_AddPRReview final : public FGitHubToolsHttpRequest < FString >
{
public:
    explicit FGitHubToolsHttpRequestData_AddPRReview( const FString & pull_request_id, EGitHubToolsPullRequestReviewEvent event );

    FString GetBody() const override;

private:
    void ParseResponse( FHttpResponsePtr response_ptr ) override;

    FString PullRequestId;
    EGitHubToolsPullRequestReviewEvent Event;
};