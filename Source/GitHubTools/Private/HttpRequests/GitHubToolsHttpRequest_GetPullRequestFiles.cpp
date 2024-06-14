#include "GitHubToolsHttpRequest_GetPullRequestFiles.h"

#include <Interfaces/IHttpResponse.h>

#define LOCTEXT_NAMESPACE "GitHubTools.Requests"

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

FText FGitHubToolsHttpRequestData_GetPullRequestFiles::GetNotificationText() const
{
    return LOCTEXT( "GetPullRequestNumber_GetPullRequestFiles", "Fetching the files updated in the pull request" );
}

FText FGitHubToolsHttpRequestData_GetPullRequestFiles::GetFailureText() const
{
    return LOCTEXT( "GetPullRequestNumber_GetPullRequestFiles", "Error while fetching the files updated in the pull request" );
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
        //filenames.Emplace( MakeShared< FGithubToolsPullRequestFileInfos >( file_infos_object->GetStringField( TEXT( "filename" ) ), file_infos_object->GetStringField( TEXT( "status" ) ) ) );
    }

    Files = filenames;
}

#undef LOCTEXT_NAMESPACE