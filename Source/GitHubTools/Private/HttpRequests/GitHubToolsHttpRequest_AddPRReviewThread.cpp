#include "GitHubToolsHttpRequest_AddPRReviewThread.h"

#include "GitHubToolsTypes.h"

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
    string_builder << TEXT( "        resolvedBy {" );
    string_builder << TEXT( "          login" );
    string_builder << TEXT( "        } " );
    string_builder << TEXT( "        id" );
    string_builder << TEXT( "        isResolved" );
    string_builder << TEXT( "        path" );
    string_builder << TEXT( "        pullRequest {" );
    string_builder << TEXT( "           number" );
    string_builder << TEXT( "        }" );
    string_builder << TEXT( "        comments( first : 100 ) {" );
    string_builder << TEXT( "          edges {" );
    string_builder << TEXT( "            node {" );
    string_builder << TEXT( "              author {" );
    string_builder << TEXT( "                login" );
    string_builder << TEXT( "              } " );
    string_builder << TEXT( "              id" );
    string_builder << TEXT( "              body" );
    string_builder << TEXT( "              createdAt" );
    string_builder << TEXT( "            }" );
    string_builder << TEXT( "          }" );
    string_builder << TEXT( "        }" );
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
    const auto pr_object = thread_object->GetObjectField( TEXT( "pullRequest" ) );

    auto review_thread_infos = MakeShared< FGithubToolsPullRequestReviewThreadInfos >();
    review_thread_infos->Id = thread_object->GetStringField( TEXT( "id" ) );
    review_thread_infos->bIsResolved = false;
    review_thread_infos->FileName = thread_object->GetStringField( TEXT( "path" ) );
    review_thread_infos->PRNumber = pr_object->GetIntegerField( TEXT( "number" ) );

    const auto comments_object = thread_object->GetObjectField( TEXT( "comments" ) );
    const auto comments_edges_object = comments_object->GetArrayField( TEXT( "edges" ) );

    review_thread_infos->Comments.Reserve( comments_edges_object.Num() );

    for ( const auto comment_object : comments_edges_object )
    {
        const auto comment_node_object = comment_object->AsObject()->GetObjectField( TEXT( "node" ) );
        const auto comment_author_object = comment_node_object->GetObjectField( TEXT( "author" ) );

        auto comment = MakeShared< FGithubToolsPullRequestComment >();

        comment->Id = comment_node_object->GetStringField( TEXT( "id" ) );
        comment->Author = FText::FromString( comment_author_object->GetStringField( TEXT( "login" ) ) );
        comment->Comment = FText::FromString( comment_node_object->GetStringField( TEXT( "body" ) ) );
        comment->Date = FText::FromString( comment_node_object->GetStringField( TEXT( "createdAt" ) ) );

        review_thread_infos->Comments.Emplace( comment );
    }

    ThreadInfos = review_thread_infos;
}

#undef LOCTEXT_NAMESPACE