#pragma once

#include "GitHubToolsHttpRequestManager.h"

class FGitHubToolsHttpRequestData_ResolveReviewThread final : public FGitHubToolsHttpRequest< bool >
{
public:
    explicit FGitHubToolsHttpRequestData_ResolveReviewThread( const FString & thread_id );

    FString GetBody() const override;

private:
    void ParseResponse( FHttpResponsePtr response_ptr ) override;

    FString ThreadId;
};