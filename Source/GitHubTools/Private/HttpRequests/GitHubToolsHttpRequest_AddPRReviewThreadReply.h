#pragma once

#include "GitHubToolsHttpRequestManager.h"
#include "GitHubToolsTypes.h"

class FGitHubToolsHttpRequestData_AddPRReviewThreadReply final : public FGitHubToolsHttpRequest< FGithubToolsPullRequestCommentPtr >
{
public:
    explicit FGitHubToolsHttpRequestData_AddPRReviewThreadReply( const FString & thread_id, const FString & comment );

    FString GetBody() const override;

private:
    void ParseResponse( FHttpResponsePtr response_ptr ) override;

    FString ThreadId;
    FString Comment;
};