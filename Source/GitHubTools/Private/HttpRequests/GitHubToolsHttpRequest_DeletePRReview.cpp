#include "GitHubToolsHttpRequest_DeletePRReview.h"

#include "Dom/JsonValue.h"
#include "GitHubToolsGitUtils.h"
#include "Serialization/JsonSerializer.h"

#define LOCTEXT_NAMESPACE "GitHubTools.Requests"

FGitHubToolsHttpRequestData_DeletePRReview::FGitHubToolsHttpRequestData_DeletePRReview( const FString & pull_request_review_id ) :
    PullRequestReviewId( pull_request_review_id )
{
}

FString FGitHubToolsHttpRequestData_DeletePRReview::GetRawQuery() const
{
    return R"(
mutation DeletePullRequestReview( 
  $pullRequestReviewId: ID!
  ) {
  deletePullRequestReview( input: { 
    pullRequestReviewId: $pullRequestReviewId
    } ) { 
      pullRequestReview {
        id
      }
    }
  }
)";
}

void FGitHubToolsHttpRequestData_DeletePRReview::ParseResponseData( const FJsonObject & json_data )
{
    const auto result_object = json_data.GetObjectField( TEXT( "dismissPullRequestReview" ) );
    const auto thread_object = result_object->GetObjectField( TEXT( "pullRequestReview" ) );
    Result = thread_object->GetStringField( TEXT( "id" ) );
}

void FGitHubToolsHttpRequestData_DeletePRReview::AddParameters( FJsonObject & variables_object ) const
{
    FGitHubToolsHttpRequestGraphQLMutation< FString >::AddParameters( variables_object );
    variables_object.SetStringField( TEXT( "pullRequestReviewId" ), PullRequestReviewId );
}

#undef LOCTEXT_NAMESPACE