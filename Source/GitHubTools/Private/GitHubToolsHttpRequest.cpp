#include "GitHubToolsHttpRequest.h"

#include "GitSourceControlModule.h"
#include "Interfaces/IHttpResponse.h"

EGitHubToolsRequestType FGitHubToolsHttpRequestData_GetPullRequestNumber::GetVerb() const
{
    return EGitHubToolsRequestType::GET;
}

FString FGitHubToolsHttpRequestData_GetPullRequestNumber::GetEndPoint() const
{
    return TEXT( "pulls" );
}

void FGitHubToolsHttpResponseData_GetPullRequestNumber::ParseResponse( FHttpResponsePtr response_ptr )
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
            PullRequestNumber = number;
            return;
        }
    }

    PullRequestNumber = INDEX_NONE;
}

FGitHubToolsHttpRequestData_GetPullRequestFiles::FGitHubToolsHttpRequestData_GetPullRequestFiles( int pull_request_number ) :
    PullRequestNumber( pull_request_number )
{
}

EGitHubToolsRequestType FGitHubToolsHttpRequestData_GetPullRequestFiles::GetVerb() const
{
    return EGitHubToolsRequestType::GET;
}

FString FGitHubToolsHttpRequestData_GetPullRequestFiles::GetEndPoint() const
{
    return FString::Printf( TEXT( "pulls/%i/files" ), PullRequestNumber );
}

void FGitHubToolsHttpResponseData_GetPullRequestFiles::ParseResponse( FHttpResponsePtr response_ptr )
{
    const auto json_response = response_ptr->GetContentAsString();
    const auto json_reader = TJsonReaderFactory<>::Create( json_response );

    TArray< TSharedPtr< FJsonValue > > files_infos;
    if ( !FJsonSerializer::Deserialize( json_reader, files_infos ) )
    {
        return;
    }

    TArray< FGithubToolsPullRequestFileInfosPtr > filenames;
    filenames.Reserve( files_infos.Num() );

    for ( const auto file_infos : files_infos )
    {
        const auto file_infos_object = file_infos->AsObject();
        filenames.Emplace( MakeShared< FGithubToolsPullRequestFileInfos >( file_infos_object->GetStringField( TEXT( "filename" ) ), file_infos_object->GetStringField( TEXT( "status" ) ) ) );
    }

    Files = filenames;
}
