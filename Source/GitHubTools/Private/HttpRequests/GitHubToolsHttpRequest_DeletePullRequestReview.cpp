#include "GitHubToolsHttpRequest_DeletePullRequestReview.h"

#include "Dom/JsonValue.h"
#include "Interfaces/IHttpResponse.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

#define LOCTEXT_NAMESPACE "GitHubTools.Requests"

FGitHubToolsHttpRequest_DeletePullRequestReview::FGitHubToolsHttpRequest_DeletePullRequestReview( const FString & review_id ) :
    ReviewId( review_id )
{
}

FString FGitHubToolsHttpRequest_DeletePullRequestReview::GetRawQuery() const
{
    return R"(
mutation DeletePullRequestReview( 
  pullRequestReviewId: ID!
  ) {
    deletePullRequestReview( 
      input: { 
        pullRequestReviewId: $pullRequestReviewId 
      } 
  ) { 
    pullRequestReview {
      id
      state
    }
  }
}
)";
}

void FGitHubToolsHttpRequest_DeletePullRequestReview::ParseResponseData( const FJsonObject & json_data )
{
    Result = true;
}

void FGitHubToolsHttpRequest_DeletePullRequestReview::AddParameters( FJsonObject & variables_object ) const
{
    FGitHubToolsHttpRequestGraphQLMutation< bool >::AddParameters( variables_object );
    variables_object.SetStringField( TEXT( "pullRequestReviewId" ), ReviewId );
}

#undef LOCTEXT_NAMESPACE