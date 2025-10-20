#pragma once

#include "GitHubToolsHttpRequestManager.h"
#include "GitHubToolsTypes.h"

class FGitHubToolsHttpRequestData_AddPRReview final : public FGitHubToolsHttpRequestGraphQLMutation< FString >
{
public:
    FGitHubToolsHttpRequestData_AddPRReview( const FString & pull_request_id, EGitHubToolsPullRequestReviewEvent event );
    explicit FGitHubToolsHttpRequestData_AddPRReview( const FString & pull_request_id );

protected:
    FString GetRawQuery() const override;
    void ParseResponseData( const FJsonObject & json_data ) override;
private:

    FString PullRequestId;
    TOptional< EGitHubToolsPullRequestReviewEvent > Event;
};