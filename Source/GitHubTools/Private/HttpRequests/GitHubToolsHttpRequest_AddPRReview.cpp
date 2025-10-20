#include "GitHubToolsHttpRequest_AddPRReview.h"

#include "Dom/JsonValue.h"
#include "GitHubToolsGitUtils.h"
#include "Serialization/JsonSerializer.h"

#define LOCTEXT_NAMESPACE "GitHubTools.Requests"

FGitHubToolsHttpRequestData_AddPRReview::FGitHubToolsHttpRequestData_AddPRReview( const FString & pull_request_id, const EGitHubToolsPullRequestReviewEvent event ) :
    PullRequestId( pull_request_id ),
    Event( event )
{
}

FGitHubToolsHttpRequestData_AddPRReview::FGitHubToolsHttpRequestData_AddPRReview( const FString & pull_request_id ) :
    PullRequestId( pull_request_id )
{
}

FString FGitHubToolsHttpRequestData_AddPRReview::GetRawQuery() const
{
    return R"(
mutation AddPullRequestReview( 
  $pullRequestId: ID!, 
  $path: String! 
  ) {
    addPullRequestReview( 
      input: { 
        pullRequestId: $pullRequestId 
      } 
  ) { 
    pullRequest {
      id
    }
  }
}
)";
}

void FGitHubToolsHttpRequestData_AddPRReview::ParseResponseData( const FJsonObject & json_data )
{
    const auto result_object = json_data.GetObjectField( TEXT( "addPullRequestReview" ) );
    const auto thread_object = result_object->GetObjectField( TEXT( "pullRequestReview" ) );
    Result = thread_object->GetStringField( TEXT( "id" ) );
}

void FGitHubToolsHttpRequestData_AddPRReview::AddParameters( FJsonObject & variables_object ) const
{
    FGitHubToolsHttpRequestGraphQLMutation::AddParameters( variables_object );

    variables_object.SetStringField( TEXT( "pullRequestId" ), PullRequestId );
}

#undef LOCTEXT_NAMESPACE