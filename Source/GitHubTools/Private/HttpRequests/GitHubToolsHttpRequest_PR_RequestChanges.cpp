#include "GitHubToolsHttpRequest_PR_RequestChanges.h"

#include "Dom/JsonValue.h"
#include "GitHubToolsGitUtils.h"
#include "Serialization/JsonSerializer.h"

#define LOCTEXT_NAMESPACE "GitHubTools.Requests"

FGitHubToolsHttpRequestData_PR_RequestChanges::FGitHubToolsHttpRequestData_PR_RequestChanges( const FString & pull_request_id ) :
    PullRequestId( pull_request_id )
{
}

FString FGitHubToolsHttpRequestData_PR_RequestChanges::GetRawQuery() const
{
    return R"(
mutation PullRequestRequestChanges( 
  $pullRequestId: ID!, 
  ) {
  submitPullRequestReview( input: { 
    pullRequestId: $pullRequestId,
    event: REQUEST_CHANGES,
    body: "Changes are requested"  
    } ) { 
      pullRequestReview {
        id
      }
    }
  }
)";
}

void FGitHubToolsHttpRequestData_PR_RequestChanges::ParseResponseData( const FJsonObject & json_data )
{
    const auto result_object = json_data.GetObjectField( TEXT( "submitPullRequestReview" ) );
    const auto thread_object = result_object->GetObjectField( TEXT( "pullRequestReview" ) );
    Result = thread_object->GetStringField( TEXT( "id" ) );
}

void FGitHubToolsHttpRequestData_PR_RequestChanges::AddParameters( FJsonObject & variables_object ) const
{
    FGitHubToolsHttpRequestGraphQLMutation< FString >::AddParameters( variables_object );
    variables_object.SetStringField( TEXT( "pullRequestId" ), PullRequestId );
}

#undef LOCTEXT_NAMESPACE