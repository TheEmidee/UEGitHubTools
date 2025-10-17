#pragma once

#include "GitHubToolsHttpRequestManager.h"
#include "GitHubToolsTypes.h"

class FGitHubToolsHttpRequestData_GetPullRequestFiles final : public FGitHubToolsHttpRequestGraphQLQueryWithPagination< TArray< FGithubToolsPullRequestFileInfosPtr > >
{
public:
    typedef TArray< FGithubToolsPullRequestFileInfosPtr > ResponseType;

    explicit FGitHubToolsHttpRequestData_GetPullRequestFiles( int pull_request_number, const FString & after_cursor = TEXT( "" ) );

    FString GetRawQuery() const override;
    void ParseResponse( FHttpResponsePtr response_ptr ) override;
    void AddParameters( TSharedPtr< FJsonObject > & variables_object ) const override;

private:
    int PullRequestNumber;
};