#pragma once

#include "GitHubToolsHttpRequestsTypes.h"

class FGitHubToolsHttpRequest_MergePR final : public FGitHubToolsHttpRequestGraphQLMutation< bool >
{
public:
    FGitHubToolsHttpRequest_MergePR( const FString & pull_request_id );

protected:
    FString GetRawQuery() const override;
    void ParseResponseData( const FJsonObject & json_data ) override;
    void AddParameters( FJsonObject & variables_object ) const override;

private:
    FString PullRequestId;
};