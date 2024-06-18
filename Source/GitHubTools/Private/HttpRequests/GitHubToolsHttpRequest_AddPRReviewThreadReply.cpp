#include "GitHubToolsHttpRequest_AddPRReviewThreadReply.h"

#include "GitHubToolsTypes.h"

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
    string_builder << TEXT( "  \"mutation {" );
    string_builder << TEXT( "    addPullRequestReviewThreadReply( input: {" );
    string_builder << TEXT( "      pullRequestReviewThreadId : \\\"" ) << *ThreadId << TEXT( "\\\"," );
    string_builder << TEXT( "      body : \\\"" ) << *Comment << TEXT( "\\\"" );
    string_builder << TEXT( "    } ) {" );
    string_builder << TEXT( "      comment {" );
    string_builder << TEXT( "        author {" );
    string_builder << TEXT( "          login" );
    string_builder << TEXT( "        } " );
    string_builder << TEXT( "        id" );
    string_builder << TEXT( "        body" );
    string_builder << TEXT( "        createdAt" );
    string_builder << TEXT( "      }" );
    string_builder << TEXT( "    }" );
    string_builder << TEXT( "  }\"" );
    string_builder << TEXT( "}" );

    return *string_builder;
}

void FGitHubToolsHttpRequestData_AddPRReviewThreadReply::ParseResponse( FHttpResponsePtr response_ptr )
{
    const auto json_response = response_ptr->GetContentAsString();
    const auto json_reader = TJsonReaderFactory<>::Create( json_response );

    TSharedPtr< FJsonValue > data;
    if ( !FJsonSerializer::Deserialize( json_reader, data ) )
    {
        return;
    }

    const auto data_object = data->AsObject()->GetObjectField( TEXT( "data" ) );
    const auto result_object = data_object->GetObjectField( TEXT( "addPullRequestReviewThreadReply" ) );
    const auto comment_object = result_object->GetObjectField( TEXT( "comment" ) );

    const auto comment_author_object = comment_object->GetObjectField( TEXT( "author" ) );

    auto comment = MakeShared< FGithubToolsPullRequestComment >();

    comment->Id = comment_object->GetStringField( TEXT( "id" ) );
    comment->Author = FText::FromString( comment_author_object->GetStringField( TEXT( "login" ) ) );
    comment->Comment = FText::FromString( comment_object->GetStringField( TEXT( "body" ) ) );
    comment->Date = FText::FromString( comment_object->GetStringField( TEXT( "createdAt" ) ) );

    Result = comment;
}

#undef LOCTEXT_NAMESPACE