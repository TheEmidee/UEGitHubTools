#pragma once

#include "GitHubToolsHttpRequestsTypes.h"
#include "GitHubToolsTypes.h"

class FGitHubToolsHttpRequest_PR_AddReviewThread : public FGitHubToolsHttpRequestGraphQLMutation< FGithubToolsPullRequestReviewThreadInfosPtr >
{

protected:
    FString GetRawQuery() const override;
    FGitHubToolsHttpRequest_PR_AddReviewThread( const FString & pull_request_id, const FString & pull_request_review_id, const FString & file_path, const FString & comment );
    void ParseResponseData( const FJsonObject & json_data ) override;
    void AddParameters( FJsonObject & variables_object ) const override;
    virtual FString GetMutationAdditionalParameters() const
    {
        return TEXT( "" );
    }
    virtual FString GetInputAdditionalParameters() const
    {
        return TEXT( "" );
    }

private:
    FString PullRequestId;
    FString PullRequestReviewId;
    FString FilePath;
    FString Comment;
};

class FGitHubToolsHttpRequest_PR_AddReviewThreadToFile final : public FGitHubToolsHttpRequest_PR_AddReviewThread
{
public:
    FGitHubToolsHttpRequest_PR_AddReviewThreadToFile( const FString & pull_request_id, const FString & pull_request_review_id, const FString & file_path, const FString & comment );

protected:
    void AddParameters( FJsonObject & variables_object ) const override;
};

class FGitHubToolsHttpRequest_PR_AddReviewThreadToLine final : public FGitHubToolsHttpRequest_PR_AddReviewThread
{
public:
    FGitHubToolsHttpRequest_PR_AddReviewThreadToLine( const FString & pull_request_id, const FString & pull_request_review_id, const FString & file_path, const EGitHubToolsDiffSide diff_side, const int line, const FString & comment );

protected:
    void AddParameters( FJsonObject & variables_object ) const override;

    FString GetMutationAdditionalParameters() const override;
    FString GetInputAdditionalParameters() const override;

private:
    EGitHubToolsDiffSide DiffSide;
    int Line;
};