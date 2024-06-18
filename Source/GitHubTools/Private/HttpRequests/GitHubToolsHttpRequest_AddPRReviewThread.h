#pragma once

#include "GitHubToolsHttpRequestManager.h"
#include "GitHubToolsTypes.h"

class FGitHubToolsHttpRequestData_AddPRReviewThread : public FGitHubToolsHttpRequest < FGithubToolsPullRequestReviewThreadInfosPtr >
{
public:
    explicit FGitHubToolsHttpRequestData_AddPRReviewThread( const FString & pull_request_id, const FString & pull_request_review_id, const FString & file_path, const FString & comment );

    FString GetBody() const override;
    bool UsesGraphQL() const override;

private:
    void ParseResponse( FHttpResponsePtr response_ptr ) override;

    FString PullRequestId;
    FString PullRequestReviewId;
    FString FilePath;
    FString Comment;
};