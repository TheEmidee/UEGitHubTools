#pragma once

#include "GitHubToolsHttpRequestsTypes.h"
#include "GitHubToolsTypes.h"

class FGitHubToolsHttpRequest_PR_Approve final : public FGitHubToolsHttpRequestGraphQLMutation< FString >
{
public:
    explicit FGitHubToolsHttpRequest_PR_Approve( const FString & pull_request_id );

protected:
    FString GetRawQuery() const override;
    void ParseResponseData( const FJsonObject & json_data ) override;
    void AddParameters( FJsonObject & variables_object ) const override;

private:
    FString PullRequestId;
};