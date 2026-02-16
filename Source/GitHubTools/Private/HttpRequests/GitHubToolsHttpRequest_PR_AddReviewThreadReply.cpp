#include "GitHubToolsHttpRequest_PR_AddReviewThreadReply.h"

#include "Dom/JsonValue.h"
#include "GitHubToolsTypes.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

#define LOCTEXT_NAMESPACE "GitHubTools.Requests"

FGitHubToolsHttpRequest_PR_AddReviewThreadReply::FGitHubToolsHttpRequest_PR_AddReviewThreadReply( const FString & thread_id, const FString & comment ) :
    ThreadId( thread_id ),
    Comment( comment )
{
}

FString FGitHubToolsHttpRequest_PR_AddReviewThreadReply::GetRawQuery() const
{
    return R"(
mutation AddPullRequestReviewThreadReply( 
  $pullRequestReviewThreadId: ID!, 
  $body: String! 
  ) {
  addPullRequestReviewThreadReply( 
    input: { 
      pullRequestReviewThreadId: $pullRequestReviewThreadId, 
      body: $body 
      } 
  ) { 
    comment {
      author {
        login
      } 
      id
      body
      createdAt
    }
  }
}
)";
}

void FGitHubToolsHttpRequest_PR_AddReviewThreadReply::ParseResponseData( const FJsonObject & json_data )
{
    const auto result_object = json_data.GetObjectField( TEXT( "addPullRequestReviewThreadReply" ) );
    const auto comment_object = result_object->GetObjectField( TEXT( "comment" ) );

    Result = MakeShared< FGithubToolsPullRequestComment >( comment_object.ToSharedRef() );
}

void FGitHubToolsHttpRequest_PR_AddReviewThreadReply::AddParameters( FJsonObject & variables_object ) const
{
    FGitHubToolsHttpRequestGraphQLMutation< TSharedPtr< FGithubToolsPullRequestComment > >::AddParameters( variables_object );

    variables_object.SetStringField( TEXT( "pullRequestReviewThreadId" ), ThreadId );
    variables_object.SetStringField( TEXT( "body" ), Comment );
}

#undef LOCTEXT_NAMESPACE