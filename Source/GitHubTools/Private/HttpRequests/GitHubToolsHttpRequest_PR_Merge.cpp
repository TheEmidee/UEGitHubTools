#include "GitHubToolsHttpRequest_PR_Merge.h"

#include "Dom/JsonValue.h"
#include "Serialization/JsonSerializer.h"

#define LOCTEXT_NAMESPACE "GitHubTools.Requests"

FGitHubToolsHttpRequest_PR_Merge::FGitHubToolsHttpRequest_PR_Merge( const FString & pull_request_id ) :
    PullRequestId( pull_request_id )
{
}

FString FGitHubToolsHttpRequest_PR_Merge::GetRawQuery() const
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

void FGitHubToolsHttpRequest_PR_Merge::ParseResponseData( const FJsonObject & json_data )
{
    Result = true;
}

void FGitHubToolsHttpRequest_PR_Merge::AddParameters( FJsonObject & variables_object ) const
{
    FGitHubToolsHttpRequestGraphQLMutation::AddParameters( variables_object );

    variables_object.SetStringField( TEXT( "pullRequestId" ), PullRequestId );
}

#undef LOCTEXT_NAMESPACE