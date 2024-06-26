#include "GitHubToolsHttpRequest_MarkFileAsViewed.h"

#include <Interfaces/IHttpResponse.h>

#define LOCTEXT_NAMESPACE "GitHubTools.Requests"

FGitHubToolsHttpRequest_MarkFileAsViewed::FGitHubToolsHttpRequest_MarkFileAsViewed( const FString & pull_request_id, const FString & path ) :
    PullRequestId( pull_request_id ),
    Path( path )
{
}

FString FGitHubToolsHttpRequest_MarkFileAsViewed::GetBody() const
{
    TStringBuilder< 512 > string_builder;

    string_builder << TEXT( "{ \"query\" :" );
    string_builder << TEXT( "  \"mutation {" );
    string_builder << TEXT( "    markFileAsViewed( input: {" );
    string_builder << TEXT( "      pullRequestId: \\\"" ) << *PullRequestId << TEXT( "\\\", " );
    string_builder << TEXT( "      path: \\\"" ) << *Path << TEXT( "\\\", " );
    string_builder << TEXT( "    } ) { " );
    string_builder << TEXT( "      pullRequest {" );
    string_builder << TEXT( "        id" );
    string_builder << TEXT( "      }" );
    string_builder << TEXT( "    }" );
    string_builder << TEXT( "  }\"" );
    string_builder << TEXT( "}" );

    return *string_builder;
}

void FGitHubToolsHttpRequest_MarkFileAsViewed::ParseResponse( FHttpResponsePtr response_ptr )
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