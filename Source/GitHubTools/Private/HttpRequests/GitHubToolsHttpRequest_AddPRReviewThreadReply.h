#pragma once

#include "GitHubToolsHttpRequestManager.h"

class FGitHubToolsHttpRequestData_AddPRReviewThreadReply : public FGitHubToolsHttpRequestData
{
public:
    explicit FGitHubToolsHttpRequestData_AddPRReviewThreadReply( const FString & thread_id, const FString & comment );

    FString GetBody() const override;
    bool UsesGraphQL() const override;
    FText GetNotificationText() const override;
    FText GetFailureText() const override;

private:
    FString ThreadId;
    FString Comment;
};

class FGitHubToolsHttpResponseData_AddPRReviewThreadReply final : public FGitHubToolsHttpResponseData
{
public:
    
    void ParseResponse( FHttpResponsePtr response_ptr ) override;
};