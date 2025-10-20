#include "GitHubToolsHttpRequest_SubmitPRReview.h"

#include "Dom/JsonValue.h"
#include "GitHubToolsGitUtils.h"
#include "Interfaces/IHttpResponse.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

#define LOCTEXT_NAMESPACE "GitHubTools.Requests"

FGitHubToolsHttpRequestData_SubmitPRReview::FGitHubToolsHttpRequestData_SubmitPRReview( const FString & pull_request_id, const FString & pull_request_review_id, EGitHubToolsPullRequestReviewEvent event ) :
    PullRequestId( pull_request_id ),
    PullRequestReviewId( pull_request_review_id ),
    Event( event )
{
}

FString FGitHubToolsHttpRequestData_SubmitPRReview::GetRawQuery() const
{
    TStringBuilder< 512 > string_builder;

    string_builder << TEXT( "{ \"query\" :" );
    string_builder << TEXT( "  \"mutation {" );
    string_builder << TEXT( "    submitPullRequestReview( input: {" );
    string_builder << TEXT( "      pullRequestId: \\\"" ) << *PullRequestId << TEXT( "\\\", " );
    string_builder << TEXT( "      pullRequestReviewId: \\\"" ) << *PullRequestReviewId << TEXT( "\\\", " );
    string_builder << TEXT( "      event: " ) << *GitHubToolsUtils::GetPullRequestReviewEventStringValue( Event ) << TEXT( ", " );
    string_builder << TEXT( "    } ) { " );
    string_builder << TEXT( "      pullRequestReview {" );
    string_builder << TEXT( "        id" );
    string_builder << TEXT( "      }" );
    string_builder << TEXT( "    }" );
    string_builder << TEXT( "  }\"" );
    string_builder << TEXT( "}" );

    return *string_builder;
}

void FGitHubToolsHttpRequestData_SubmitPRReview::ParseResponseData( const FJsonObject & json_data )
{
    const auto result_object = json_data.GetObjectField( TEXT( "submitPullRequestReview" ) );
    const auto thread_object = result_object->GetObjectField( TEXT( "pullRequestReview" ) );
    Result = thread_object->GetStringField( TEXT( "id" ) );
}

#undef LOCTEXT_NAMESPACE