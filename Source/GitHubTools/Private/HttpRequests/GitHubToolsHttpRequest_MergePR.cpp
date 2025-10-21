#include "GitHubToolsHttpRequest_MergePR.h"

#include "Dom/JsonValue.h"
#include "Serialization/JsonSerializer.h"

#define LOCTEXT_NAMESPACE "GitHubTools.Requests"

FGitHubToolsHttpRequest_MergePR::FGitHubToolsHttpRequest_MergePR( const FString & pull_request_id ) :
    PullRequestId( pull_request_id )
{
}

FString FGitHubToolsHttpRequest_MergePR::GetRawQuery() const
{
    return R"(
mutation MergePullRequest( 
  $pullRequestId: ID! 
  ) {
  mergePullRequest( 
    input: { 
      pullRequestId: $pullRequestId, 
      mergeMethod: MERGE 
    } 
  ) { 
    pullRequest {
      id
      }
  }
}
)";
}

void FGitHubToolsHttpRequest_MergePR::ParseResponseData( const FJsonObject & json_data )
{
    Result = true;
}

void FGitHubToolsHttpRequest_MergePR::AddParameters( FJsonObject & variables_object ) const
{
    FGitHubToolsHttpRequestGraphQLMutation::AddParameters( variables_object );

    variables_object.SetStringField( TEXT( "pullRequestId" ), PullRequestId );
}

#undef LOCTEXT_NAMESPACE