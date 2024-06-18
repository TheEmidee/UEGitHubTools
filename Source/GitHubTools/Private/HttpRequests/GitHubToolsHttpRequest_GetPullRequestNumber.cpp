#include "GitHubToolsHttpRequest_GetPullRequestNumber.h"

#include "GitSourceControlModule.h"

#include <Interfaces/IHttpResponse.h>

#define LOCTEXT_NAMESPACE "GitHubTools.Requests"

EGitHubToolsRequestType FGitHubToolsHttpRequestData_GetPullRequestNumber::GetVerb() const
{
    return EGitHubToolsRequestType::GET;
}

FString FGitHubToolsHttpRequestData_GetPullRequestNumber::GetEndPoint() const
{
    return TEXT( "pulls" );
}

void FGitHubToolsHttpRequestData_GetPullRequestNumber::ParseResponse( FHttpResponsePtr response_ptr )
{
    const auto local_branch_name = FGitSourceControlModule::Get().GetProvider().GetBranchName();
    const auto json_response = response_ptr->GetContentAsString();
    const auto json_reader = TJsonReaderFactory<>::Create( json_response );

    TArray< TSharedPtr< FJsonValue > > pull_requests_infos;
    if ( !FJsonSerializer::Deserialize( json_reader, pull_requests_infos ) )
    {
        return;
    }

    for ( const auto pull_request_infos : pull_requests_infos )
    {
        const auto pr_object = pull_request_infos->AsObject();
        const int number = pr_object->GetIntegerField( TEXT( "number" ) );
        const auto head_object = pr_object->GetObjectField( TEXT( "head" ) );
        const auto ref = head_object->GetStringField( TEXT( "ref" ) );

        if ( ref == local_branch_name )
        {
            Result = number;
            return;
        }
    }

    Result = INDEX_NONE;
}

#undef LOCTEXT_NAMESPACE