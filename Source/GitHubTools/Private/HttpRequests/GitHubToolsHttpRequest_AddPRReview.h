#pragma once

#include "GitHubToolsHttpRequestManager.h"
#include "GitHubToolsTypes.h"

class FGitHubToolsHttpRequestData_AddPRReview final : public FGitHubToolsHttpRequest< FString >
{
public:
    FGitHubToolsHttpRequestData_AddPRReview( const FString & pull_request_id, EGitHubToolsPullRequestReviewEvent event );
    explicit FGitHubToolsHttpRequestData_AddPRReview( const FString & pull_request_id );

    FString GetBody() const override;

private:
    void ParseResponse( FHttpResponsePtr response_ptr ) override;

    FString PullRequestId;
    TOptional< EGitHubToolsPullRequestReviewEvent > Event;
};