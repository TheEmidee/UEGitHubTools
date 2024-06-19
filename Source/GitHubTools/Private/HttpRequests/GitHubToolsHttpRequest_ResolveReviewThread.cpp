#include "GitHubToolsHttpRequest_ResolveReviewThread.h"

#include <Interfaces/IHttpResponse.h>

#define LOCTEXT_NAMESPACE "GitHubTools.Requests"

FGitHubToolsHttpRequestData_ResolveReviewThread::FGitHubToolsHttpRequestData_ResolveReviewThread( const FString & thread_id ) :
    ThreadId( thread_id )
{
}

FString FGitHubToolsHttpRequestData_ResolveReviewThread::GetBody() const
{
    TStringBuilder< 512 > string_builder;

    string_builder << TEXT( "{ \"query\" :" );
    string_builder << TEXT( "  \"mutation ( $input:ResolveReviewThreadInput! ) {" );
    string_builder << TEXT( "    resolveReviewThread( input: $input ) {" );
    string_builder << TEXT( "      thread {" );
    string_builder << TEXT( "        id" );
    string_builder << TEXT( "      }" );
    string_builder << TEXT( "    }" );
    string_builder << TEXT( "  }\"" );
    string_builder << TEXT( "  ," );
    string_builder << TEXT( "  \"variables\" : {" );
    string_builder << TEXT( "    \"input\" : {" );
    string_builder << TEXT( "      \"threadId\" : \"" ) << *ThreadId << TEXT( "\"" );
    string_builder << TEXT( "    }" );
    string_builder << TEXT( "  }" );
    string_builder << TEXT( "}" );

    return *string_builder;
}

void FGitHubToolsHttpRequestData_ResolveReviewThread::ParseResponse( FHttpResponsePtr response_ptr )
{
    const auto json_response = response_ptr->GetContentAsString();
    const auto json_reader = TJsonReaderFactory<>::Create( json_response );

    TSharedPtr< FJsonValue > data;
    if ( !FJsonSerializer::Deserialize( json_reader, data ) )
    {
        return;
    }

    Result = true;
}

#undef LOCTEXT_NAMESPACE