#include "GitHubToolsHttpRequest_AddPRReviewThreadReply.h"

#include "Dom/JsonValue.h"
#include "GitHubToolsTypes.h"
#include "Interfaces/IHttpResponse.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

#define LOCTEXT_NAMESPACE "GitHubTools.Requests"

FGitHubToolsHttpRequestData_AddPRReviewThreadReply::FGitHubToolsHttpRequestData_AddPRReviewThreadReply( const FString & thread_id, const FString & comment ) :
    ThreadId( thread_id ),
    Comment( comment )
{
}

FString FGitHubToolsHttpRequestData_AddPRReviewThreadReply::GetRawQuery() const
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

void FGitHubToolsHttpRequestData_AddPRReviewThreadReply::ParseResponseData( const FJsonObject & json_data )
{
    const auto result_object = json_data.GetObjectField( TEXT( "addPullRequestReviewThreadReply" ) );
    const auto comment_object = result_object->GetObjectField( TEXT( "comment" ) );
    const auto comment_author_object = comment_object->GetObjectField( TEXT( "author" ) );

    auto comment = MakeShared< FGithubToolsPullRequestComment >();

    comment->Id = comment_object->GetStringField( TEXT( "id" ) );
    comment->Author = FText::FromString( comment_author_object->GetStringField( TEXT( "login" ) ) );
    comment->Comment = FText::FromString( comment_object->GetStringField( TEXT( "body" ) ) );
    comment->Date = FText::FromString( comment_object->GetStringField( TEXT( "createdAt" ) ) );

    Result = comment;
}

void FGitHubToolsHttpRequestData_AddPRReviewThreadReply::AddParameters( FJsonObject & variables_object ) const
{
    FGitHubToolsHttpRequestGraphQLMutation< TSharedPtr< FGithubToolsPullRequestComment > >::AddParameters( variables_object );

    variables_object.SetStringField( TEXT( "pullRequestReviewThreadId" ), ThreadId );
    variables_object.SetStringField( TEXT( "body" ), Comment );
}

#undef LOCTEXT_NAMESPACE