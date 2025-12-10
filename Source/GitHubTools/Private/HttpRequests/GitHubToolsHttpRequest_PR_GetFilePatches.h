#pragma once

#include "GitHubToolsHttpRequestsTypes.h"
#include "GitHubToolsTypes.h"

class FGitHubToolsHttpRequest_PR_GetFilePatches final : public FGitHubToolsHttpRequestRestQuery< TArray< FGithubToolsPullRequestFilePatchPtr > >
{
public:
    typedef TArray< bool > ResponseType;

    explicit FGitHubToolsHttpRequest_PR_GetFilePatches( int pull_request_number );

protected:
    FString GetEndPoint() const override;
    void ParseResponseData( const FJsonValue & json_data ) override;

private:
    int PullRequestNumber;
};