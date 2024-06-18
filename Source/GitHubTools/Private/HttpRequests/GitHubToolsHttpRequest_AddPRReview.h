#pragma once

#include "GitHubToolsHttpRequestManager.h"

class FGitHubToolsHttpRequestData_AddPRReview final : public FGitHubToolsHttpRequest < FString >
{
public:
    explicit FGitHubToolsHttpRequestData_AddPRReview( const FString & pull_request_id );

    FString GetBody() const override;
    bool UsesGraphQL() const override;

private:
    void ParseResponse( FHttpResponsePtr response_ptr ) override;

    FString PullRequestId;
};