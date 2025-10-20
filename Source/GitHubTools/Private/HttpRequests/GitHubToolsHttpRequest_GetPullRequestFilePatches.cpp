#include "HttpRequests/GitHubToolsHttpRequest_GetPullRequestFilePatches.h"

#include "Dom/JsonValue.h"
#include "Serialization/JsonSerializer.h"

#define LOCTEXT_NAMESPACE "GitHubTools.Requests"

FGitHubToolsHttpRequestData_GetPullRequestFilePatches::FGitHubToolsHttpRequestData_GetPullRequestFilePatches( int pull_request_number ) :
    PullRequestNumber( pull_request_number )
{
}

FString FGitHubToolsHttpRequestData_GetPullRequestFilePatches::GetEndPoint() const
{
    return FString::Printf( TEXT( "pulls/%i/files?per_page=100" ), PullRequestNumber );
}

void FGitHubToolsHttpRequestData_GetPullRequestFilePatches::ParseResponseData( const FJsonValue & json_data )
{
    const auto objects = json_data.AsArray();

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