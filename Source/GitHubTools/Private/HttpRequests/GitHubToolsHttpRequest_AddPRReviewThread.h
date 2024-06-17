#pragma once

#include "GitHubToolsHttpRequestManager.h"

class FGitHubToolsHttpRequestData_AddPRReviewThread : public FGitHubToolsHttpRequestData
{
public:
    explicit FGitHubToolsHttpRequestData_AddPRReviewThread( const FString & pull_request_id, const FString & pull_request_review_id, const FString & file_path, const FString & comment );

    FString GetBody() const override;
    bool UsesGraphQL() const override;
    FText GetNotificationText() const override;
    FText GetFailureText() const override;

private:
    FString PullRequestId;
    FString PullRequestReviewId;
    FString FilePath;
    FString Comment;
};

class FGitHubToolsHttpResponseData_AddPRReviewThread final : public FGitHubToolsHttpResponseData
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