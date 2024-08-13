#include "GitHubToolsHttpRequest_GetPullRequestFilePatches.h"

#include <Interfaces/IHttpResponse.h>

#define LOCTEXT_NAMESPACE "GitHubTools.Requests"

FGitHubToolsHttpRequestData_GetPullRequestFilePatches::FGitHubToolsHttpRequestData_GetPullRequestFilePatches( int pull_request_number ) :
    PullRequestNumber( pull_request_number )
{
}

FString FGitHubToolsHttpRequestData_GetPullRequestFilePatches::GetEndPoint() const
{
    return FString::Printf( TEXT( "pulls/%i/files?per_page=100" ), PullRequestNumber );
}

bool FGitHubToolsHttpRequestData_GetPullRequestFilePatches::UsesGraphQL() const
{
    return false;
}

void FGitHubToolsHttpRequestData_GetPullRequestFilePatches::ParseResponse( FHttpResponsePtr response_ptr )
{
    const auto json_response = response_ptr->GetContentAsString();
    const auto json_reader = TJsonReaderFactory<>::Create( json_response );

    TSharedPtr< FJsonValue > data;
    if ( !FJsonSerializer::Deserialize( json_reader, data ) )
    {
        return;
    }

    const auto objects = data->AsArray();

    TArray< FGithubToolsPullRequestFilePatchPtr > patches;
    patches.Reserve( objects.Num() );

    for ( auto array_object : objects )
    {
        const auto object = array_object->AsObject();
        const auto file_name = object->GetStringField( TEXT( "filename" ) );
        const auto patch = object->GetStringField( TEXT( "patch" ) );

        patches.Add( MakeShared< FGithubToolsPullRequestFilePatch >( file_name, patch ) );
    }

    Result = patches;
}

#undef LOCTEXT_NAMESPACE