#pragma once

#include "GitHubToolsHttpRequestsTypes.h"

class FGitHubToolsHttpRequest_DeletePullRequestReview final : public FGitHubToolsHttpRequestGraphQLMutation< bool >
{
public:
    explicit FGitHubToolsHttpRequest_DeletePullRequestReview( const FString & review_id );

protected:
    FString GetRawQuery() const override;
    void ParseResponseData( const FJsonObject & json_data ) override;
    void AddParameters( FJsonObject & variables_object ) const override;

private:
    FString ReviewId;
};