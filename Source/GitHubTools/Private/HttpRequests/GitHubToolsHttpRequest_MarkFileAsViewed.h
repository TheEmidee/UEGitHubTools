#pragma once

#include "GitHubToolsHttpRequestManager.h"

class FGitHubToolsHttpRequest_MarkFileAsViewed : public FGitHubToolsHttpRequest< bool >
{
public:
    FGitHubToolsHttpRequest_MarkFileAsViewed( const FString & pull_request_id, const FString & path );

    FString GetBody() const override;

private:
    void ParseResponse( FHttpResponsePtr response_ptr ) override;

    FString PullRequestId;
    FString Path;
};