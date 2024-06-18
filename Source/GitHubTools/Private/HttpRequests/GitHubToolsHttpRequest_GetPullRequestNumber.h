#pragma once

#include "GitHubToolsHttpRequestManager.h"

class FGitHubToolsHttpRequestData_GetPullRequestNumber : public FGitHubToolsHttpRequest< int >
{
public:
    EGitHubToolsRequestType GetVerb() const override;
    FString GetEndPoint() const override;

private:
    void ParseResponse( FHttpResponsePtr response_ptr ) override;
};