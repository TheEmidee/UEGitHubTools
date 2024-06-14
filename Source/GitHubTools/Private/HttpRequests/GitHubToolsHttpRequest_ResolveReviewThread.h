#pragma once

#include "GitHubToolsHttpRequestManager.h"

class FGitHubToolsHttpRequestData_ResolveReviewThread : public FGitHubToolsHttpRequestData
{
public:
    explicit FGitHubToolsHttpRequestData_ResolveReviewThread( const FString & thread_id );

    FString GetBody() const override;
    bool UsesGraphQL() const override;
    FText GetNotificationText() const override;
    FText GetFailureText() const override;

private:
    FString ThreadId;
};

class FGitHubToolsHttpResponseData_ResolveReviewThread final : public FGitHubToolsHttpResponseData
{
public:
    
    void ParseResponse( FHttpResponsePtr response_ptr ) override;

private:
    
};