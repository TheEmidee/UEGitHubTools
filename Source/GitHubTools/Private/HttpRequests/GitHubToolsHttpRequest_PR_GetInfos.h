#pragma once

#include "GitHubToolsHttpRequestsTypes.h"
#include "GitHubToolsTypes.h"

class FGitHubToolsHttpRequest_PR_GetInfos final : public FGitHubToolsHttpRequestGraphQLQuery< FGithubToolsPullRequestInfosPtr >
{
public:
    typedef FGithubToolsPullRequestInfosPtr ResponseType;

    explicit FGitHubToolsHttpRequest_PR_GetInfos( int pull_request_number, TArray< FGithubToolsPullRequestFileInfosPtr > files, TArray< FGithubToolsPullRequestFilePatchPtr > patches );

protected:
    FString GetRawQuery() const override;
    void AddParameters( FJsonObject & variables_object ) const override;
    void ParseResponseData( const FJsonObject & json_data ) override;

private:
    int PullRequestNumber;
    TArray< FGithubToolsPullRequestFileInfosPtr > Files;
    TArray< FGithubToolsPullRequestFilePatchPtr > Patches;
};