#include "GitHubToolsHttpRequest_AddPRReviewThreadReply.h"

#include <Interfaces/IHttpResponse.h>

#define LOCTEXT_NAMESPACE "GitHubTools.Requests"

FGitHubToolsHttpRequestData_AddPRReviewThreadReply::FGitHubToolsHttpRequestData_AddPRReviewThreadReply( const FString & thread_id, const FString & comment ) :
    ThreadId( thread_id ),
    Comment( comment )
{
}

bool FGitHubToolsHttpRequestData_AddPRReviewThreadReply::UsesGraphQL() const
{
    return true;
}

FString FGitHubToolsHttpRequestData_AddPRReviewThreadReply::GetBody() const
{
    TStringBuilder< 512 > string_builder;

    string_builder << TEXT( "{ \"query\" :" );
    string_builder << TEXT( "  \"mutation ( $input:AddPullRequestReviewThreadReplyInput! ) {" );
    string_builder << TEXT( "    addPullRequestReviewThreadReply( input: $input ) {" );
    string_builder << TEXT( "      comment {" );
    string_builder << TEXT( "        id" );
    string_builder << TEXT( "      }" );
    string_builder << TEXT( "    }" );
    string_builder << TEXT( "  }\"" );
    string_builder << TEXT( "  ," );
    string_builder << TEXT( "  \"variables\" : {" );
    string_builder << TEXT( "    \"input\" : {" );
    string_builder << TEXT( "      \"pullRequestReviewThreadId\" : \"" ) << *ThreadId << TEXT( "\"," );
    string_builder << TEXT( "      \"body\" : \"" ) << *Comment << TEXT( "\"" );
    string_builder << TEXT( "    }" );
    string_builder << TEXT( "  }" );
    string_builder << TEXT( "}" );

    return *string_builder;
}

FText FGitHubToolsHttpRequestData_AddPRReviewThreadReply::GetNotificationText() const
{
    return LOCTEXT( "ddPRReviewThreadReply", "Adding a reply to the thread" );
}

FText FGitHubToolsHttpRequestData_AddPRReviewThreadReply::GetFailureText() const
{
    return LOCTEXT( "ddPRReviewThreadReply", "Error while adding a reply to the thread" );
}

void FGitHubToolsHttpResponseData_AddPRReviewThreadReply::ParseResponse( FHttpResponsePtr response_ptr )
{
    const auto json_response = response_ptr->GetContentAsString();
    const auto json_reader = TJsonReaderFactory<>::Create( json_response );

    TSharedPtr< FJsonValue > data;
    if ( !FJsonSerializer::Deserialize( json_reader, data ) )
    {
        return;
    }
}

#undef LOCTEXT_NAMESPACE