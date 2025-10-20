#include "GitHubToolsHttpRequest_ResolveReviewThread.h"

#include "Dom/JsonValue.h"
#include "Interfaces/IHttpResponse.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

#define LOCTEXT_NAMESPACE "GitHubTools.Requests"

FGitHubToolsHttpRequestData_ResolveReviewThread::FGitHubToolsHttpRequestData_ResolveReviewThread( const FString & thread_id ) :
    ThreadId( thread_id )
{
}

FString FGitHubToolsHttpRequestData_ResolveReviewThread::GetRawQuery() const
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

void FGitHubToolsHttpRequestData_ResolveReviewThread::ParseResponseData( const FJsonObject & json_data )
{
    Result = true;
}

#undef LOCTEXT_NAMESPACE