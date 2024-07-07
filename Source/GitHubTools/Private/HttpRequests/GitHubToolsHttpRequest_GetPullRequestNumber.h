#pragma once

#include "GitHubToolsHttpRequestManager.h"

class FGitHubToolsHttpRequestData_GetPullRequestNumber final : public FGitHubToolsHttpRequest< int >
{
public:
    FString GetBody() const override;

private:
    void ParseResponse( FHttpResponsePtr response_ptr ) override;
};