#pragma once

#include "GitHubToolsHttpRequestsTypes.h"

class FGitHubToolsHttpRequestData_ResolveReviewThread final : public FGitHubToolsHttpRequestGraphQLMutation< bool >
{
public:
    explicit FGitHubToolsHttpRequestData_ResolveReviewThread( const FString & thread_id );

protected:
    FString GetRawQuery() const override;
    void ParseResponseData( const FJsonObject & json_data ) override;
    void AddParameters( FJsonObject & variables_object ) const override;

private:
    FString ThreadId;
};