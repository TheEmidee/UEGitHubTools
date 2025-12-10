#include "GitHubToolsHttpRequest_PR_DismissReview.h"

#include "Dom/JsonValue.h"
#include "GitHubToolsGitUtils.h"
#include "Serialization/JsonSerializer.h"

#define LOCTEXT_NAMESPACE "GitHubTools.Requests"

FGitHubToolsHttpRequest_PR_DismissReview::FGitHubToolsHttpRequest_PR_DismissReview( const FString & pull_request_review_id ) :
    PullRequestReviewId( pull_request_review_id )
{
}

FString FGitHubToolsHttpRequest_PR_DismissReview::GetRawQuery() const
{
    return R"(
mutation DismissPullRequestReview( 
  $pullRequestReviewId: ID!,
  $message: String!,
  ) {
  dismissPullRequestReview( input: { 
    pullRequestReviewId: $pullRequestReviewId,
    message: $message
    } ) { 
      pullRequestReview {
        id
      }
    }
  }
)";
}

void FGitHubToolsHttpRequest_PR_DismissReview::ParseResponseData( const FJsonObject & json_data )
{
    const auto result_object = json_data.GetObjectField( TEXT( "dismissPullRequestReview" ) );
    const auto thread_object = result_object->GetObjectField( TEXT( "pullRequestReview" ) );
    Result = thread_object->GetStringField( TEXT( "id" ) );
}

void FGitHubToolsHttpRequest_PR_DismissReview::AddParameters( FJsonObject & variables_object ) const
{
    FGitHubToolsHttpRequestGraphQLMutation< FString >::AddParameters( variables_object );
    variables_object.SetStringField( TEXT( "pullRequestReviewId" ), PullRequestReviewId );
    variables_object.SetStringField( TEXT( "message" ), TEXT( "Dismissed" ) );
}

#undef LOCTEXT_NAMESPACE