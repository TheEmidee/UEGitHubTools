#pragma once

#include "GitHubToolsHttpRequestManager.h"
#include "GitHubToolsTypes.h"

class FGitHubToolsHttpRequestData_GetPullRequestInfos : public FGitHubToolsHttpRequestData
{
public:
    explicit FGitHubToolsHttpRequestData_GetPullRequestInfos( int pull_request_number );

    FString GetBody() const override;
    bool UsesGraphQL() const override;
    FText GetNotificationText() const override;
    FText GetFailureText() const override;

private:
    int PullRequestNumber;
};

class FGitHubToolsHttpResponseData_GetPullRequestInfos final : public FGitHubToolsHttpResponseData
{
public:
    /*FORCEINLINE TOptional< TArray< FGithubToolsPullRequestReviewInfosPtr > > GetPullRequestReviews() const
    {
        return Reviews;
    }*/

    void ParseResponse( FHttpResponsePtr response_ptr ) override;

private:
    //TOptional< TArray< FGithubToolsPullRequestReviewInfosPtr > > Reviews;
};