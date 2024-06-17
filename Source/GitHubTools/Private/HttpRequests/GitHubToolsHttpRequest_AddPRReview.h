#pragma once

#include "GitHubToolsHttpRequestManager.h"

class FGitHubToolsHttpRequestData_AddPRReview : public FGitHubToolsHttpRequestData
{
public:
    explicit FGitHubToolsHttpRequestData_AddPRReview( const FString & pull_request_id );

    FString GetBody() const override;
    bool UsesGraphQL() const override;
    FText GetNotificationText() const override;
    FText GetFailureText() const override;

private:
    FString PullRequestId;
};

class FGitHubToolsHttpResponseData_AddPRReview final : public FGitHubToolsHttpResponseData
{
public:
    FORCEINLINE TOptional< FString > GetThreadId() const
    {
        return ThreadId;
    }

    void ParseResponse( FHttpResponsePtr response_ptr ) override;

private:
    TOptional< FString > ThreadId;
};