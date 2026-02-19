#pragma once

#include "GitHubToolsHttpRequestsTypes.h"
#include "GitHubToolsTypes.h"

class FGitHubToolsHttpRequest_PR_GetFilePatches final : public FGitHubToolsHttpRequestRestQueryWithPagination< TArray< FGithubToolsPullRequestFilePatchPtr > >
{
public:
    typedef TArray< FGithubToolsPullRequestFilePatchPtr > ResponseType;

    FGitHubToolsHttpRequest_PR_GetFilePatches( int pull_request_number, int page_index = 1 );

protected:
    FString GetEndPoint() const override;
    void ParseResponseData( const FJsonValue & json_data ) override;

private:
    int PullRequestNumber;
};