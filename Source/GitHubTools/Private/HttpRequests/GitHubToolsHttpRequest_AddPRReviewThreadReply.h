#pragma once

#include "GitHubToolsHttpRequestsTypes.h"
#include "GitHubToolsTypes.h"

class FGitHubToolsHttpRequestData_AddPRReviewThreadReply final : public FGitHubToolsHttpRequestGraphQLMutation< FGithubToolsPullRequestCommentPtr >
{
public:
    explicit FGitHubToolsHttpRequestData_AddPRReviewThreadReply( const FString & thread_id, const FString & comment );

protected:
    FString GetRawQuery() const override;
    void ParseResponseData( const FJsonObject & json_data ) override;
    void AddParameters( FJsonObject & variables_object ) const override;

private:
    FString ThreadId;
    FString Comment;
};