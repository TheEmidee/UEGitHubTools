#pragma once

#include "GitHubToolsHttpRequestsTypes.h"
#include "GitHubToolsTypes.h"

class FGitHubToolsHttpRequest_PR_AddComment final : public FGitHubToolsHttpRequestGraphQLMutation< FGithubToolsPullRequestCommentPtr >
{
public:
    explicit FGitHubToolsHttpRequest_PR_AddComment( const FString & thread_id, const FString & comment );

protected:
    FString GetRawQuery() const override;
    void ParseResponseData( const FJsonObject & json_data ) override;
    void AddParameters( FJsonObject & variables_object ) const override;

private:
    FString PullRequestId;
    FString Comment;
};