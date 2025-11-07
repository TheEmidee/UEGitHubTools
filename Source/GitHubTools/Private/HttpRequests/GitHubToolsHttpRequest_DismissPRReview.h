#pragma once

#include "GitHubToolsHttpRequestsTypes.h"
#include "GitHubToolsTypes.h"

class FGitHubToolsHttpRequestData_DismissPRReview final : public FGitHubToolsHttpRequestGraphQLMutation< FString >
{
public:
    explicit FGitHubToolsHttpRequestData_DismissPRReview( const FString & pull_request_review_id );

protected:
    FString GetRawQuery() const override;
    void ParseResponseData( const FJsonObject & json_data ) override;
    void AddParameters( FJsonObject & variables_object ) const override;

private:
    FString PullRequestReviewId;
};