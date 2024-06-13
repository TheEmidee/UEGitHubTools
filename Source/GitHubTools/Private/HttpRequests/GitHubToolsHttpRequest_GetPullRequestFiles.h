#pragma once

#include "GitHubToolsHttpRequestManager.h"
#include "GitHubToolsTypes.h"

class FGitHubToolsHttpRequestData_GetPullRequestFiles : public FGitHubToolsHttpRequestData
{
public:
    explicit FGitHubToolsHttpRequestData_GetPullRequestFiles( int pull_request_number );
    EGitHubToolsRequestType GetVerb() const override;
    FString GetEndPoint() const override;
    FText GetNotificationText() const override;
    FText GetFailureText() const override;

private:
    int PullRequestNumber;
};

class FGitHubToolsHttpResponseData_GetPullRequestFiles final : public FGitHubToolsHttpResponseData
{
public:
    FORCEINLINE TOptional< TArray< FGithubToolsPullRequestFileInfosPtr > > GetPullRequestFiles() const
    {
        return Files;
    }

    void ParseResponse( FHttpResponsePtr response_ptr ) override;

private:
    TOptional< TArray< FGithubToolsPullRequestFileInfosPtr > > Files;
};