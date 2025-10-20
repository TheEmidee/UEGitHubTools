#include "GitHubToolsHttpRequest_ResolveReviewThread.h"

#include "Dom/JsonValue.h"
#include "Serialization/JsonSerializer.h"

#define LOCTEXT_NAMESPACE "GitHubTools.Requests"

FGitHubToolsHttpRequestData_ResolveReviewThread::FGitHubToolsHttpRequestData_ResolveReviewThread( const FString & thread_id ) :
    ThreadId( thread_id )
{
}

FString FGitHubToolsHttpRequestData_ResolveReviewThread::GetRawQuery() const
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

void FGitHubToolsHttpRequestData_ResolveReviewThread::ParseResponseData( const FJsonObject & json_data )
{
    Result = true;
}

void FGitHubToolsHttpRequestData_ResolveReviewThread::AddParameters( FJsonObject & variables_object ) const
{
    FGitHubToolsHttpRequestGraphQLMutation< bool >::AddParameters( variables_object );
    variables_object.SetStringField( TEXT( "threadId" ), ThreadId );
}

#undef LOCTEXT_NAMESPACE