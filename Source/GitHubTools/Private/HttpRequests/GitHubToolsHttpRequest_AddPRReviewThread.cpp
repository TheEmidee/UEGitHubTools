#include "GitHubToolsHttpRequest_AddPRReviewThread.h"

#include <Interfaces/IHttpResponse.h>

#define LOCTEXT_NAMESPACE "GitHubTools.Requests"

FGitHubToolsHttpRequestData_AddPRReviewThread::FGitHubToolsHttpRequestData_AddPRReviewThread( const FString & pull_request_id, const FString & pull_request_review_id, const FString & file_path, const FString & comment ) :
    PullRequestId( pull_request_id ),
    PullRequestReviewId( pull_request_review_id ),
    FilePath( file_path ),
    Comment( comment )
{
}

bool FGitHubToolsHttpRequestData_AddPRReviewThread::UsesGraphQL() const
{
    return true;
}

FString FGitHubToolsHttpRequestData_AddPRReviewThread::GetBody() const
{
    TStringBuilder< 512 > string_builder;

    string_builder << TEXT( "{ \"query\" :" );
    string_builder << TEXT( "  \"mutation {" );
    string_builder << TEXT( "    addPullRequestReviewThread( input: {" );
    string_builder << TEXT( "      body: \\\"" ) << *Comment << TEXT( "\\\", " );
    string_builder << TEXT( "      pullRequestId: \\\"" ) << *PullRequestId << TEXT( "\\\", " );
    string_builder << TEXT( "      pullRequestReviewId: \\\"" ) << *PullRequestReviewId << TEXT( "\\\", " );
    string_builder << TEXT( "      subjectType: FILE, " );
    string_builder << TEXT( "      path: \\\"" ) << *FilePath << TEXT( "\\\" " );
    string_builder << TEXT( "    } ) { " );
    string_builder << TEXT( "      thread {" );
    string_builder << TEXT( "        id" );
    string_builder << TEXT( "      }" );
    string_builder << TEXT( "    }" );
    string_builder << TEXT( "  }\"" );
    string_builder << TEXT( "}" );

    return *string_builder;
}

FText FGitHubToolsHttpRequestData_AddPRReviewThread::GetNotificationText() const
{
    return LOCTEXT( "AddPRReviewThread", "Creating new thread" );
}

FText FGitHubToolsHttpRequestData_AddPRReviewThread::GetFailureText() const
{
    return LOCTEXT( "AddPRReviewThread", "Error while creating new thread" );
}

void FGitHubToolsHttpResponseData_AddPRReviewThread::ParseResponse( FHttpResponsePtr response_ptr )
{
    const auto json_response = response_ptr->GetContentAsString();
    const auto json_reader = TJsonReaderFactory<>::Create( json_response );

    TSharedPtr< FJsonValue > data;
    if ( !FJsonSerializer::Deserialize( json_reader, data ) )
    {
        return;
    }

    const auto data_object = data->AsObject()->GetObjectField( TEXT( "data" ) );
    const auto result_object = data_object->GetObjectField( TEXT( "addPullRequestReviewThread" ) );
    const auto thread_object = result_object->GetObjectField( TEXT( "thread" ) );
    ThreadId = thread_object->GetStringField( TEXT( "id" ) );
}

#undef LOCTEXT_NAMESPACE