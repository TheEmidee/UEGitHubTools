#pragma once

#include "GitHubToolsHttpRequestsTypes.h"
#include "GitHubToolsTypes.h"

class FGitHubToolsHttpRequestData_GetPullRequestFilePatches final : public FGitHubToolsHttpRequestRestQuery< TArray< FGithubToolsPullRequestFilePatchPtr > >
{
public:
    typedef TArray< bool > ResponseType;

    explicit FGitHubToolsHttpRequestData_GetPullRequestFilePatches( int pull_request_number );

protected:
    FString GetEndPoint() const override;
    void ParseResponseData( const FJsonValue & json_data ) override;

private:
    int PullRequestNumber;
};