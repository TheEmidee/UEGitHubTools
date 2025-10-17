#pragma once

#include "GitHubToolsHttpRequestManager.h"
#include "GitHubToolsTypes.h"

class FGitHubToolsHttpRequest_GetOpenedPullRequests final : public FGitHubToolsHttpRequestQueryGraphQL< TArray< FGitHubToolsOpenedPullRequestInfosPtr > >
{
public:
    using ResponseType = TArray< FGitHubToolsOpenedPullRequestInfosPtr >;

    FString GetRawQuery() const override;

private:
    void ParseResponse( FHttpResponsePtr response_ptr ) override;
};