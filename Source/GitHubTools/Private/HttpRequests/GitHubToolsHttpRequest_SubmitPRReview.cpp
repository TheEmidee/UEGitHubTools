#include "GitHubToolsHttpRequest_SubmitPRReview.h"

#include "Dom/JsonValue.h"
#include "GitHubToolsGitUtils.h"
#include "Serialization/JsonSerializer.h"

#define LOCTEXT_NAMESPACE "GitHubTools.Requests"

FGitHubToolsHttpRequestData_SubmitPRReview::FGitHubToolsHttpRequestData_SubmitPRReview( const FString & pull_request_id, const FString & pull_request_review_id, EGitHubToolsPullRequestReviewEvent event ) :
    PullRequestId( pull_request_id ),
    PullRequestReviewId( pull_request_review_id ),
    Event( event )
{
}

FString FGitHubToolsHttpRequestData_SubmitPRReview::GetRawQuery() const
{
    return R"(
mutation SubmitPullRequestReview( 
  $pullRequestId: ID!, 
  $pullRequestReviewId: ID!, 
  $event: PullRequestReviewEvent! 
  ) {
  submitPullRequestReview( input: { 
    pullRequestId: $pullRequestId, 
    pullRequestReviewId: $pullRequestReviewId, 
    event: $event  
    } ) { 
      pullRequestReview {
        id
      }
    }
  }
)";
}

void FGitHubToolsHttpRequestData_SubmitPRReview::ParseResponseData( const FJsonObject & json_data )
{
    const auto result_object = json_data.GetObjectField( TEXT( "submitPullRequestReview" ) );
    const auto thread_object = result_object->GetObjectField( TEXT( "pullRequestReview" ) );
    Result = thread_object->GetStringField( TEXT( "id" ) );
}

void FGitHubToolsHttpRequestData_SubmitPRReview::AddParameters( FJsonObject & variables_object ) const
{
    FGitHubToolsHttpRequestGraphQLMutation< FString >::AddParameters( variables_object );
    variables_object.SetStringField( TEXT( "pullRequestId" ), PullRequestId );
    variables_object.SetStringField( TEXT( "pullRequestReviewId" ), PullRequestReviewId );
    variables_object.SetStringField( TEXT( "event" ), *GitHubToolsUtils::GetPullRequestReviewEventStringValue( Event ) );
}

#undef LOCTEXT_NAMESPACE