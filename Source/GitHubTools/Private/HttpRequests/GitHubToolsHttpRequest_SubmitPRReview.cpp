#include "GitHubToolsHttpRequest_SubmitPRReview.h"

#include <Interfaces/IHttpResponse.h>

#define LOCTEXT_NAMESPACE "GitHubTools.Requests"

namespace
{
    FString GetPullRequestReviewEventStringValue( EGitHubToolsPullRequestReviewEvent event )
    {
        switch ( event )
        {

            case EGitHubToolsPullRequestReviewEvent::Approve:
                return TEXT( "APPROVE" );
            case EGitHubToolsPullRequestReviewEvent::Comment:
                return TEXT( "COMMENT" );
            case EGitHubToolsPullRequestReviewEvent::Dismiss:
                return TEXT( "DISMISS" );
            case EGitHubToolsPullRequestReviewEvent::RequestChanges:
                return TEXT( "REQUEST_CHANGES" );
            default:
            {
                checkNoEntry();
            };
        }
        return TEXT( "" );
    }
}

FGitHubToolsHttpRequestData_SubmitPRReview::FGitHubToolsHttpRequestData_SubmitPRReview( const FString & pull_request_id, const FString & pull_request_review_id, EGitHubToolsPullRequestReviewEvent event ) :
    PullRequestId( pull_request_id ),
    PullRequestReviewId( pull_request_review_id ),
    Event( event )
{
}

FString FGitHubToolsHttpRequestData_SubmitPRReview::GetBody() const
{
    TStringBuilder< 512 > string_builder;

    string_builder << TEXT( "{ \"query\" :" );
    string_builder << TEXT( "  \"mutation {" );
    string_builder << TEXT( "    submitPullRequestReview( input: {" );
    string_builder << TEXT( "      pullRequestId: \\\"" ) << *PullRequestId << TEXT( "\\\", " );
    string_builder << TEXT( "      pullRequestReviewId: \\\"" ) << *PullRequestReviewId << TEXT( "\\\", " );
    string_builder << TEXT( "      event: " ) << *GetPullRequestReviewEventStringValue( Event ) << TEXT( ", " );
    string_builder << TEXT( "    } ) { " );
    string_builder << TEXT( "      pullRequestReview {" );
    string_builder << TEXT( "        id" );
    string_builder << TEXT( "      }" );
    string_builder << TEXT( "    }" );
    string_builder << TEXT( "  }\"" );
    string_builder << TEXT( "}" );

    return *string_builder;
}

void FGitHubToolsHttpRequestData_SubmitPRReview::ParseResponse( FHttpResponsePtr response_ptr )
{
    const auto json_response = response_ptr->GetContentAsString();
    const auto json_reader = TJsonReaderFactory<>::Create( json_response );

    TSharedPtr< FJsonValue > data;
    if ( !FJsonSerializer::Deserialize( json_reader, data ) )
    {
        return;
    }

    const auto data_object = data->AsObject()->GetObjectField( TEXT( "data" ) );
    const auto result_object = data_object->GetObjectField( TEXT( "submitPullRequestReview" ) );
    const auto thread_object = result_object->GetObjectField( TEXT( "pullRequestReview" ) );
    Result = thread_object->GetStringField( TEXT( "id" ) );
}

#undef LOCTEXT_NAMESPACE