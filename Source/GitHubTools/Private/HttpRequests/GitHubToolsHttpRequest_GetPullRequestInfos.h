#pragma once

#include "GitHubToolsHttpRequestManager.h"
#include "GitHubToolsTypes.h"

class FGitHubToolsHttpRequestData_GetPullRequestInfos : public FGitHubToolsHttpRequest < FGithubToolsPullRequestInfosPtr >
{
public:
    typedef FGithubToolsPullRequestInfosPtr ResponseType;

    explicit FGitHubToolsHttpRequestData_GetPullRequestInfos( int pull_request_number );

    FString GetBody() const override;
    bool UsesGraphQL() const override;
    FText GetNotificationText() const override;
    FText GetFailureText() const override;
    void ParseResponse( FHttpResponsePtr response_ptr ) override;

private:
    int PullRequestNumber;
};

//class FGitHubToolsHttpResponseData_GetPullRequestInfos final : public FGitHubToolsHttpResponseData
//{
//public:
//    FORCEINLINE TOptional< FGithubToolsPullRequestInfosPtr > GetPullRequestInfos() const
//    {
//        return PRInfos;
//    }
//
//    void ParseResponse( FHttpResponsePtr response_ptr ) override;
//
//private:
//    TOptional< FGithubToolsPullRequestInfosPtr > PRInfos;
//};