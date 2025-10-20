#pragma once

#include "GitHubToolsHttpRequestManager.h"
#include "GitHubToolsTypes.h"

class FGitHubToolsHttpRequestData_AddPRReviewThread : public FGitHubToolsHttpRequestGraphQLMutation< FGithubToolsPullRequestReviewThreadInfosPtr >
{
public:
    FString GetRawQuery() const override;

protected:
    FGitHubToolsHttpRequestData_AddPRReviewThread( const FString & pull_request_id, const FString & pull_request_review_id, const FString & file_path, const FString & comment );

    virtual FString GetMutationInputData() const = 0;

private:
    void ParseResponseData( const FJsonObject & json_data ) override;

    FString PullRequestId;
    FString PullRequestReviewId;
    FString FilePath;
    FString Comment;
};

class FGitHubToolsHttpRequestData_AddPRReviewThreadToFile final : public FGitHubToolsHttpRequestData_AddPRReviewThread
{
public:
    FGitHubToolsHttpRequestData_AddPRReviewThreadToFile( const FString & pull_request_id, const FString & pull_request_review_id, const FString & file_path, const FString & comment );

protected:
    FString GetMutationInputData() const override;
};

class FGitHubToolsHttpRequestData_AddPRReviewThreadToLine final : public FGitHubToolsHttpRequestData_AddPRReviewThread
{
public:
    FGitHubToolsHttpRequestData_AddPRReviewThreadToLine( const FString & pull_request_id, const FString & pull_request_review_id, const FString & file_path, const EGitHubToolsDiffSide diff_side, const int line, const FString & comment );

protected:
    FString GetMutationInputData() const override;

private:
    EGitHubToolsDiffSide DiffSide;
    int Line;
};