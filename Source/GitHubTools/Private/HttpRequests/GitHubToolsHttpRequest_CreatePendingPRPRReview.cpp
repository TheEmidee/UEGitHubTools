#include "GitHubToolsHttpRequest_CreatePendingPRPRReview.h"

#include "Dom/JsonValue.h"
#include "GitHubToolsGitUtils.h"
#include "Serialization/JsonSerializer.h"

#define LOCTEXT_NAMESPACE "GitHubTools.Requests"

FGitHubToolsHttpRequestData_CreatePendingPRReview::FGitHubToolsHttpRequestData_CreatePendingPRReview( const FString & pull_request_id ) :
    PullRequestId( pull_request_id )
{
}

FString FGitHubToolsHttpRequestData_CreatePendingPRReview::GetRawQuery() const
{
    // When the event parameter is omitted, then the PR is created as PENDING 
    return R"(
mutation AddPullRequestReview( 
  $pullRequestId: ID!
  ) {
    addPullRequestReview( 
      input: { 
        pullRequestId: $pullRequestId
      } 
  ) { 
    pullRequestReview {
      id
    }
  }
}
)";
}

void FGitHubToolsHttpRequestData_CreatePendingPRReview::ParseResponseData( const FJsonObject & json_data )
{
    const auto result_object = json_data.GetObjectField( TEXT( "addPullRequestReview" ) );
    const auto thread_object = result_object->GetObjectField( TEXT( "pullRequestReview" ) );
    Result = thread_object->GetStringField( TEXT( "id" ) );
}

void FGitHubToolsHttpRequestData_CreatePendingPRReview::AddParameters( FJsonObject & variables_object ) const
{
    FGitHubToolsHttpRequestGraphQLMutation::AddParameters( variables_object );

    variables_object.SetStringField( TEXT( "pullRequestId" ), PullRequestId );
}

#undef LOCTEXT_NAMESPACE