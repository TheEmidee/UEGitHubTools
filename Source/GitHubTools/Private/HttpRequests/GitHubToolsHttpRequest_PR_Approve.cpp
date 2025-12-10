#include "GitHubToolsHttpRequest_PR_Approve.h"

#include "Dom/JsonValue.h"
#include "GitHubToolsGitUtils.h"
#include "Serialization/JsonSerializer.h"

#define LOCTEXT_NAMESPACE "GitHubTools.Requests"

FGitHubToolsHttpRequest_PR_Approve::FGitHubToolsHttpRequest_PR_Approve( const FString & pull_request_id ) :
    PullRequestId( pull_request_id )
{
}

FString FGitHubToolsHttpRequest_PR_Approve::GetRawQuery() const
{
    return R"(
mutation AddPullRequestReview( 
  $pullRequestId: ID!, 
  ) {
  addPullRequestReview( input: { 
    pullRequestId: $pullRequestId,
    event: APPROVE  
    } ) { 
      pullRequestReview {
        id
      }
    }
  }
)";
}

void FGitHubToolsHttpRequest_PR_Approve::ParseResponseData( const FJsonObject & json_data )
{
    const auto result_object = json_data.GetObjectField( TEXT( "addPullRequestReview" ) );
    const auto thread_object = result_object->GetObjectField( TEXT( "pullRequestReview" ) );
    Result = thread_object->GetStringField( TEXT( "id" ) );
}

void FGitHubToolsHttpRequest_PR_Approve::AddParameters( FJsonObject & variables_object ) const
{
    FGitHubToolsHttpRequestGraphQLMutation< FString >::AddParameters( variables_object );
    variables_object.SetStringField( TEXT( "pullRequestId" ), PullRequestId );
}

#undef LOCTEXT_NAMESPACE