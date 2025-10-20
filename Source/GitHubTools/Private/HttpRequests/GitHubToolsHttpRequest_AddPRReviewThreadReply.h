#pragma once

#include "GitHubToolsHttpRequestManager.h"
#include "GitHubToolsTypes.h"

class FGitHubToolsHttpRequestData_AddPRReviewThreadReply final : public FGitHubToolsHttpRequestGraphQLMutation< FGithubToolsPullRequestCommentPtr >
{
public:
    explicit FGitHubToolsHttpRequestData_AddPRReviewThreadReply( const FString & thread_id, const FString & comment );

    FString GetRawQuery() const override;

private:
    void ParseResponseData( const FJsonObject & json_data ) override;

    FString ThreadId;
    FString Comment;
};