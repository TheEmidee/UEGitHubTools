#pragma once

#include "GitHubToolsHttpRequestsTypes.h"
#include "GitHubToolsTypes.h"

class FGitHubToolsHttpRequest_PR_GetFiles final : public FGitHubToolsHttpRequestGraphQLQueryWithPagination< TArray< FGithubToolsPullRequestFileInfosPtr > >
{
public:
    typedef TArray< FGithubToolsPullRequestFileInfosPtr > ResponseType;

    explicit FGitHubToolsHttpRequest_PR_GetFiles( int pull_request_number, const FString & after_cursor = TEXT( "" ) );

protected:
    FString GetRawQuery() const override;
    void ParseResponseData( const FJsonObject & json_data ) override;
    void AddParameters( FJsonObject & variables_object ) const override;

private:
    int PullRequestNumber;
};