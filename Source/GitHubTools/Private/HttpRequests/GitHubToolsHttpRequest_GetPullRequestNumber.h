#pragma once

#include "GitHubToolsHttpRequestManager.h"

class FGitHubToolsHttpRequestData_GetPullRequestNumber : public FGitHubToolsHttpRequest< int >
{
public:
    FString GetBody() const override;

private:
    void ParseResponse( FHttpResponsePtr response_ptr ) override;
};