#include "GitHubToolsHttpRequest_GetPullRequestFiles.h"

#include <Interfaces/IHttpResponse.h>

#define LOCTEXT_NAMESPACE "GitHubTools.Requests"

FGitHubToolsHttpRequestData_GetPullRequestFiles::FGitHubToolsHttpRequestData_GetPullRequestFiles( int pull_request_number, const FString & after_cursor ) :
    FGitHubToolsHttpRequestWithPagination( after_cursor ),
    PullRequestNumber( pull_request_number )
{
}

bool FGitHubToolsHttpRequestData_GetPullRequestFiles::UsesGraphQL() const
{
    return true;
}

FString FGitHubToolsHttpRequestData_GetPullRequestFiles::GetBody() const
{
    TStringBuilder< 512 > string_builder;

    string_builder << TEXT( "{ \"query\" : \"query ($repoOwner: String!, $repoName: String!, $pullNumber: Int!) {" );
    string_builder << TEXT( "  repository(owner: $repoOwner, name: $repoName) {" );
    string_builder << TEXT( "    pullRequest( number : $pullNumber ) {" );
    string_builder << TEXT( "      files(" ) << GetCursorInfo() << TEXT( " ) {" );
    string_builder << TEXT( "        nodes {" );
    string_builder << TEXT( "          path" );
    string_builder << TEXT( "          changeType" );
    string_builder << TEXT( "          viewerViewedState" );
    string_builder << TEXT( "        }" );

    string_builder << GetPageInfoJson();

    string_builder << TEXT( "      }" );
    string_builder << TEXT( "    }" );
    string_builder << TEXT( "  }" );
    string_builder << TEXT( "}" );
    string_builder << TEXT( "\"," );
    string_builder << TEXT( "\"variables\": " );
    string_builder << TEXT( "  {" );
    string_builder << TEXT( "    \"repoOwner\": \"FishingCactus\"," );
    string_builder << TEXT( "    \"repoName\": \"SummerCamp\"," );
    string_builder << TEXT( "    \"pullNumber\": " << PullRequestNumber );
    string_builder << TEXT( "  }" );
    string_builder << TEXT( "}" );

    return *string_builder;
}

void FGitHubToolsHttpRequestData_GetPullRequestFiles::ParseResponse( FHttpResponsePtr response_ptr )
{
    const auto json_response = response_ptr->GetContentAsString();
    const auto json_reader = TJsonReaderFactory<>::Create( json_response );

    TSharedPtr< FJsonValue > data;
    if ( !FJsonSerializer::Deserialize( json_reader, data ) )
    {
        return;
    }

    const auto data_object = data->AsObject()->GetObjectField( TEXT( "data" ) );
    const auto repository_object = data_object->GetObjectField( TEXT( "repository" ) );
    const auto pull_request_object = repository_object->GetObjectField( TEXT( "pullRequest" ) );

    const auto files_object = pull_request_object->GetObjectField( TEXT( "files" ) );
    const auto files_edges_object = files_object->GetArrayField( TEXT( "nodes" ) );

    TArray< FGithubToolsPullRequestFileInfosPtr > files;

    for ( const auto file_object : files_edges_object )
    {
        const auto file_node_object = file_object->AsObject();

        files.Emplace( MakeShared< FGithubToolsPullRequestFileInfos >(
            file_node_object->GetStringField( TEXT( "path" ) ),
            file_node_object->GetStringField( TEXT( "changeType" ) ),
            file_node_object->GetStringField( TEXT( "viewerViewedState" ) ) ) );
    }

    ParsePageInfo( files_object );

    Result = files;
}

#undef LOCTEXT_NAMESPACE