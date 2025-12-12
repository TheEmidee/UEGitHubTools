#pragma once

#include "GitHubToolsHttpRequestsTypes.h"
#include "GitHubToolsTypes.h"

class FGitHubToolsHttpRequestData_PR_RequestChanges final : public FGitHubToolsHttpRequestGraphQLMutation< FString >
{
public:
    explicit FGitHubToolsHttpRequestData_PR_RequestChanges( const FString & pull_request_id );

protected:
    FString GetRawQuery() const override;
    void ParseResponseData( const FJsonObject & json_data ) override;
    void AddParameters( FJsonObject & variables_object ) const override;

private:
    FString PullRequestId;
};