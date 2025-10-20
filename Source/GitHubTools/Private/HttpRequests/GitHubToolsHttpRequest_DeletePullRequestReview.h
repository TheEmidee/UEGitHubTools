#pragma once

#include "GitHubToolsHttpRequestManager.h"

class FGitHubToolsHttpRequest_DeletePullRequestReview final : public FGitHubToolsHttpRequestGraphQLMutation< bool >
{
public:
    explicit FGitHubToolsHttpRequest_DeletePullRequestReview( const FString & review_id );

    FString GetRawQuery() const override;

private:
    void ParseResponseData( const FJsonObject & json_data ) override;

    FString ReviewId;
};