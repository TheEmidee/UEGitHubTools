#pragma once

#include "GitHubToolsHttpRequestManager.h"

class FGitHubToolsHttpRequestData_GetPullRequestNumber : public FGitHubToolsHttpRequestData
{
public:
    EGitHubToolsRequestType GetVerb() const override;
    FString GetEndPoint() const override;
    FText GetNotificationText() const override;
    FText GetFailureText() const override;
};

class FGitHubToolsHttpResponseData_GetPullRequestNumber final : public FGitHubToolsHttpResponseData
{
public:
    FORCEINLINE TOptional< int > GetPullRequestNumber() const
    {
        return PullRequestNumber;
    }

    void ParseResponse( FHttpResponsePtr response_ptr ) override;

private:
    TOptional< int > PullRequestNumber;
};