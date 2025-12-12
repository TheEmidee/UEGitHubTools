#include "GitHubToolsHttpRequest_ReviewThread_Resolve.h"

#include "Dom/JsonValue.h"
#include "Serialization/JsonSerializer.h"

#define LOCTEXT_NAMESPACE "GitHubTools.Requests"

FGitHubToolsHttpRequest_ReviewThread_Resolve::FGitHubToolsHttpRequest_ReviewThread_Resolve( const FString & thread_id ) :
    ThreadId( thread_id )
{
}

FString FGitHubToolsHttpRequest_ReviewThread_Resolve::GetRawQuery() const
{
    return R"(
mutation ResolveReviewThread( 
  $threadId: ID! 
  ) {
  resolveReviewThread( input: { 
    threadId: $threadId 
    } ) { 
      thread {
        id
      }
    }
  }
)";
}

void FGitHubToolsHttpRequest_ReviewThread_Resolve::ParseResponseData( const FJsonObject & json_data )
{
    Result = true;
}

void FGitHubToolsHttpRequest_ReviewThread_Resolve::AddParameters( FJsonObject & variables_object ) const
{
    FGitHubToolsHttpRequestGraphQLMutation< bool >::AddParameters( variables_object );
    variables_object.SetStringField( TEXT( "threadId" ), ThreadId );
}

#undef LOCTEXT_NAMESPACE