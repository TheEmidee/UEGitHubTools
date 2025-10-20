#include "GitHubToolsHttpRequest_AddPRReview.h"

#include "Dom/JsonValue.h"
#include "GitHubToolsGitUtils.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

#define LOCTEXT_NAMESPACE "GitHubTools.Requests"

FGitHubToolsHttpRequestData_AddPRReview::FGitHubToolsHttpRequestData_AddPRReview( const FString & pull_request_id, const EGitHubToolsPullRequestReviewEvent event ) :
    PullRequestId( pull_request_id ),
    Event( event )
{
}

FGitHubToolsHttpRequestData_AddPRReview::FGitHubToolsHttpRequestData_AddPRReview( const FString & pull_request_id ) :
    PullRequestId( pull_request_id )
{
}

FString FGitHubToolsHttpRequestData_AddPRReview::GetRawQuery() const
{
    TStringBuilder< 512 > string_builder;

    string_builder << TEXT( "{ \"query\" :" );
    string_builder << TEXT( "  \"mutation {" );
    string_builder << TEXT( "    addPullRequestReview( input: {" );
    string_builder << TEXT( "      pullRequestId: \\\"" ) << *PullRequestId << TEXT( "\\\", " );

    if ( Event.IsSet() )
    {
        string_builder << TEXT( ", event: " ) << *GitHubToolsUtils::GetPullRequestReviewEventStringValue( Event.GetValue() );
    }

    string_builder << TEXT( "    } ) { " );
    string_builder << TEXT( "      pullRequestReview {" );
    string_builder << TEXT( "        id" );
    string_builder << TEXT( "      }" );
    string_builder << TEXT( "    }" );
    string_builder << TEXT( "  }\"" );
    string_builder << TEXT( "}" );

    return *string_builder;
}

void FGitHubToolsHttpRequestData_AddPRReview::ParseResponseData( const FJsonObject & json_data )
{
    const auto result_object = json_data.GetObjectField( TEXT( "addPullRequestReview" ) );
    const auto thread_object = result_object->GetObjectField( TEXT( "pullRequestReview" ) );
    Result = thread_object->GetStringField( TEXT( "id" ) );
}

#undef LOCTEXT_NAMESPACE