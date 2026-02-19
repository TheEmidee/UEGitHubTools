#include "GitHubToolsHttpRequest_PR_AddComment.h"

#include "Dom/JsonValue.h"
#include "GitHubToolsTypes.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

#define LOCTEXT_NAMESPACE "GitHubTools.Requests"

FGitHubToolsHttpRequest_PR_AddComment::FGitHubToolsHttpRequest_PR_AddComment( const FString & pr_id, const FString & comment ) :
    PullRequestId( pr_id ),
    Comment( comment )
{
}

FString FGitHubToolsHttpRequest_PR_AddComment::GetRawQuery() const
{
    return R"(
mutation AddPullRequestComment( 
  $subjectId: ID!, 
  $body: String! 
  ) {
  addComment( 
    input: { 
      subjectId: $subjectId, 
      body: $body 
      } 
  ) { 
    commentEdge {
      node {
        author {
          login
        } 
        id
        body
        createdAt
      }
    }
  }
}
)";
}

void FGitHubToolsHttpRequest_PR_AddComment::ParseResponseData( const FJsonObject & json_data )
{
    const auto result_object = json_data.GetObjectField( TEXT( "addComment" ) );
    const auto comment_object = result_object->GetObjectField( TEXT( "commentEdge" ) )->GetObjectField( TEXT( "node" ) );

    Result = MakeShared< FGithubToolsPullRequestComment >( comment_object.ToSharedRef() );
}

void FGitHubToolsHttpRequest_PR_AddComment::AddParameters( FJsonObject & variables_object ) const
{
    FGitHubToolsHttpRequestGraphQLMutation< TSharedPtr< FGithubToolsPullRequestComment > >::AddParameters( variables_object );

    variables_object.SetStringField( TEXT( "subjectId" ), PullRequestId );
    variables_object.SetStringField( TEXT( "body" ), Comment );
}

#undef LOCTEXT_NAMESPACE