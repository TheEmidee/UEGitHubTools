#include "GitHubToolsHttpRequest_GetPullRequestFiles.h"

#include "Dom/JsonValue.h"
#include "GitHubToolsSettings.h"
#include "Serialization/JsonSerializer.h"

#define LOCTEXT_NAMESPACE "GitHubTools.Requests"

FGitHubToolsHttpRequestData_GetPullRequestFiles::FGitHubToolsHttpRequestData_GetPullRequestFiles( int pull_request_number, const FString & after_cursor ) :
    FGitHubToolsHttpRequestGraphQLQueryWithPagination( after_cursor ),
    PullRequestNumber( pull_request_number )
{
}

FString FGitHubToolsHttpRequestData_GetPullRequestFiles::GetRawQuery() const
{
    return R"(
query ($repoOwner: String!, $repoName: String!, $pullNumber: Int!) {
  repository(owner: $repoOwner, name: $repoName) {
    pullRequest( number : $pullNumber ) {
      files( __CURSOR_INFO__ ) {
        nodes {
          path
          changeType
          viewerViewedState
        }
        pageInfo {
          hasNextPage
          endCursor
        }
      }
    }
  }
}
)";
}

void FGitHubToolsHttpRequestData_GetPullRequestFiles::ParseResponseData( const FJsonObject & json_data )
{
    const auto repository_object = json_data.GetObjectField( TEXT( "repository" ) );
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

    ParsePageInfo( *files_object.Get() );

    Result = files;
}

void FGitHubToolsHttpRequestData_GetPullRequestFiles::AddParameters( FJsonObject & variables_object ) const
{
    FGitHubToolsHttpRequestGraphQLQueryWithPagination< TArray< FGithubToolsPullRequestFileInfosPtr > >::AddParameters( variables_object );
    variables_object.SetNumberField( TEXT( "pullNumber" ), PullRequestNumber );
}

#undef LOCTEXT_NAMESPACE