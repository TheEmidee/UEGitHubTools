#pragma once

#include "GitHubToolsHttpRequestsTypes.h"

class FGitHubToolsHttpRequestData_ResolveReviewThread final : public FGitHubToolsHttpRequestGraphQLMutation< bool >
{
public:
    explicit FGitHubToolsHttpRequestData_ResolveReviewThread( const FString & thread_id );

    FString GetRawQuery() const override;
    

protected:
    void ParseResponseData( const FJsonObject & json_data ) override;

private:
    FString ThreadId;
};