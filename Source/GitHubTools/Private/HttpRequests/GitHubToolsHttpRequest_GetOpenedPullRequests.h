#pragma once

#include "GitHubToolsHttpRequestManager.h"
#include "GitHubToolsTypes.h"

class FGitHubToolsHttpRequest_GetOpenedPullRequests final : public FGitHubToolsHttpRequest< TArray< FGitHubToolsOpenedPullRequestInfosPtr > >
{
public:
    typedef TArray< FGitHubToolsOpenedPullRequestInfosPtr > ResponseType;
    
    FString GetBody() const override;

private:
    void ParseResponse( FHttpResponsePtr response_ptr ) override;
};