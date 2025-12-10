#pragma once

#include "GitHubToolsHttpRequestManager.h"
#include "GitHubToolsHttpRequestsTypes.h"
#include "GitHubToolsTypes.h"

class FGitHubToolsHttpRequest_Repo_GetOpenPRs final : public FGitHubToolsHttpRequestGraphQLQuery< TArray< FGitHubToolsOpenedPullRequestInfosPtr > >
{
public:
    using ResponseType = TArray< FGitHubToolsOpenedPullRequestInfosPtr >;

    FString GetRawQuery() const override;

private:
    void ParseResponseData( const FJsonObject & json_data ) override;
};