#pragma once

#include "GitHubToolsHttpRequestManager.h"

class FGitHubToolsHttpRequest_MergePR final : public FGitHubToolsHttpRequest< bool >
{
public:
    FGitHubToolsHttpRequest_MergePR( const FString & pull_request_id );

    FString GetBody() const override;

private:
    void ParseResponse( FHttpResponsePtr response_ptr ) override;

    FString PullRequestId;
};