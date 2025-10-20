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
    TStringBuilder< 512 > string_builder;

    string_builder << TEXT( "{ \"query\" :" );
    string_builder << TEXT( "  \"mutation {" );
    string_builder << TEXT( "      deletePullRequestReview( input: {" );
    string_builder << TEXT( "        pullRequestReviewId: \\\"" ) << *ReviewId << TEXT( "\\\", " );
    string_builder << TEXT( "      } ) {" );
    string_builder << TEXT( "      pullRequestReview {" );
    string_builder << TEXT( "        id" );
    string_builder << TEXT( "        state" );
    string_builder << TEXT( "      }" );
    string_builder << TEXT( "    }" );
    string_builder << TEXT( "  }\"" );
    string_builder << TEXT( "}" );

    return *string_builder;
}

void FGitHubToolsHttpRequest_DeletePullRequestReview::ParseResponseData( const FJsonObject & json_data )
{
    Result = true;
}

#undef LOCTEXT_NAMESPACE