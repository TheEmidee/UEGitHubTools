#pragma once

#include "GitHubToolsHttpRequestManager.h"

class FGitHubToolsHttpRequest_DeletePullRequestReview final : public FGitHubToolsHttpRequest< bool >
{
public:
    explicit FGitHubToolsHttpRequest_DeletePullRequestReview( const FString & review_id );

    FString GetBody() const override;

private:
    void ParseResponse( FHttpResponsePtr response_ptr ) override;

    FString ReviewId;
};