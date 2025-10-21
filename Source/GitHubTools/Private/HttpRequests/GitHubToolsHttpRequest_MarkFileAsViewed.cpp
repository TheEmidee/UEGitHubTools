#include "GitHubToolsHttpRequest_MarkFileAsViewed.h"

#include "Dom/JsonObject.h"

#define LOCTEXT_NAMESPACE "GitHubTools.Requests"

FGitHubToolsHttpRequest_MarkFileAsViewed::FGitHubToolsHttpRequest_MarkFileAsViewed( const FString & pull_request_id, const FString & path ) :
    PullRequestId( pull_request_id ),
    Path( path )
{
}

FString FGitHubToolsHttpRequest_MarkFileAsViewed::GetRawQuery() const
{
    return R"(
mutation MarkFileAsViewed( 
  $pullRequestId: ID!, 
  $path: String! 
  ) {
  markFileAsViewed( 
    input: { 
      pullRequestId: $pullRequestId, 
      path: $path 
      } 
  ) { 
    pullRequest {
      id
    }
  }
}
)";
}

void FGitHubToolsHttpRequest_MarkFileAsViewed::AddParameters( FJsonObject & variables_object ) const
{
    FGitHubToolsHttpRequestGraphQLMutation::AddParameters( variables_object );

    variables_object.SetStringField( TEXT( "pullRequestId" ), PullRequestId );
    variables_object.SetStringField( TEXT( "path" ), Path );
}

void FGitHubToolsHttpRequest_MarkFileAsViewed::ParseResponseData( const FJsonObject & json_data )
{
    Result = true;
}

#undef LOCTEXT_NAMESPACE