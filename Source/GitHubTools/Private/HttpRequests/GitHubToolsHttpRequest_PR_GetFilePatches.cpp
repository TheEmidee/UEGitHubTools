#include "HttpRequests/GitHubToolsHttpRequest_PR_GetFilePatches.h"

#include "Dom/JsonValue.h"
#include "Serialization/JsonSerializer.h"

#define LOCTEXT_NAMESPACE "GitHubTools.Requests"

FGitHubToolsHttpRequest_PR_GetFilePatches::FGitHubToolsHttpRequest_PR_GetFilePatches( int pull_request_number ) :
    PullRequestNumber( pull_request_number )
{
}

FString FGitHubToolsHttpRequest_PR_GetFilePatches::GetEndPoint() const
{
    return FString::Printf( TEXT( "pulls/%i/files?per_page=100" ), PullRequestNumber );
}

void FGitHubToolsHttpRequest_PR_GetFilePatches::ParseResponseData( const FJsonValue & json_data )
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