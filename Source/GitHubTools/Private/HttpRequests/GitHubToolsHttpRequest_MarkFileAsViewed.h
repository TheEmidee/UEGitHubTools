#pragma once

#include "GitHubToolsHttpRequestManager.h"
#include "GitHubToolsHttpRequestsTypes.h"

class FGitHubToolsHttpRequest_MarkFileAsViewed final : public FGitHubToolsHttpRequestGraphQLMutation< bool >
{
public:
    FGitHubToolsHttpRequest_MarkFileAsViewed( const FString & pull_request_id, const FString & path );

protected:
    FString GetRawQuery() const override;
    void AddParameters( FJsonObject & variables_object ) const override;
    void ParseResponseData( const FJsonObject & json_data ) override;

private:
    FString PullRequestId;
    FString Path;
};