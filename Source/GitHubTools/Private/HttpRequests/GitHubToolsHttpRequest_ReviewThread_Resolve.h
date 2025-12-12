#pragma once

#include "GitHubToolsHttpRequestsTypes.h"

class FGitHubToolsHttpRequest_ReviewThread_Resolve final : public FGitHubToolsHttpRequestGraphQLMutation< bool >
{
public:
    explicit FGitHubToolsHttpRequest_ReviewThread_Resolve( const FString & thread_id );

protected:
    FString GetRawQuery() const override;
    void ParseResponseData( const FJsonObject & json_data ) override;
    void AddParameters( FJsonObject & variables_object ) const override;

private:
    FString ThreadId;
};