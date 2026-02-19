#include "GitHubToolsHttpRequestsTypes.h"

#include "GitHubToolsSettings.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

namespace
{
    template < typename _JSON_TYPE_ >
    bool DeserializeResponseContent( const FHttpResponsePtr & response_ptr, TSharedPtr< _JSON_TYPE_ > & result )
    {
        const auto json_response = response_ptr->GetContentAsString();
        const auto json_reader = TJsonReaderFactory<>::Create( json_response );

        return FJsonSerializer::Deserialize( json_reader, result );
    }
}

template < typename TResultType >
void FGitHubToolsHttpRequest< TResultType >::SetupHttpRequest( IHttpRequest & http_request )
{
    http_request.SetVerb( GetVerb() );
    http_request.SetURL( GetURL() );
}

template < typename TResultType >
void FGitHubToolsHttpRequestGraphQL< TResultType >::SetupHttpRequest( IHttpRequest & http_request )
{
    FGitHubToolsHttpRequest< TResultType >::SetupHttpRequest( http_request );

    TSharedPtr< FJsonObject > body_object = MakeShared< FJsonObject >();
    body_object->SetStringField( TEXT( "query" ), this->GetQuery() );

    TSharedPtr< FJsonObject > variables_object = MakeShared< FJsonObject >();
    this->AddParameters( variables_object.ToSharedRef().Get() );

    body_object->SetObjectField( TEXT( "variables" ), variables_object );

    FString body;
    TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create( &body );
    if ( FJsonSerializer::Serialize( body_object.ToSharedRef(), Writer ) )
    {
        http_request.SetContentAsString( body );
    }
}

template < typename TResultType >
void FGitHubToolsHttpRequestGraphQL< TResultType >::ProcessResponse( const FHttpResponsePtr & response_ptr )
{
    TSharedPtr< FJsonObject > result;
    if ( !DeserializeResponseContent( response_ptr, result ) )
    {
        return;
    }

    const TArray< TSharedPtr< FJsonValue > > * errors;
    if ( result->TryGetArrayField( TEXT( "errors" ), errors ) )
    {
        if ( !errors->IsEmpty() )
        {
            this->ErrorMessage = ( *errors )[ 0 ]->AsObject()->GetStringField( TEXT( "message" ) );
        }
    }

    if ( this->ErrorMessage.IsEmpty() )
    {
        result->TryGetStringField( TEXT( "message" ), this->ErrorMessage );
    }

    if ( !this->ErrorMessage.IsEmpty() )
    {
        return;
    }

    ParseResponseData( *result->GetObjectField( TEXT( "data" ) ).Get() );
}

template < typename TResultType >
void FGitHubToolsHttpRequestGraphQL< TResultType >::ProcessRawQuery( FString & query ) const
{
}

template < typename TResultType >
void FGitHubToolsHttpRequestGraphQL< TResultType >::AddParameters( FJsonObject & variables_object ) const
{
}

template < typename TResultType >
FString FGitHubToolsHttpRequestGraphQL< TResultType >::GetVerb() const
{
    static const FString Verb = TEXT( "POST" );
    return Verb;
}

template < typename TResultType >
FString FGitHubToolsHttpRequestGraphQL< TResultType >::GetURL() const
{
    static const FString URL( TEXT( "https://api.github.com/graphql" ) );
    return URL;
}

template < typename TResultType >
FString FGitHubToolsHttpRequestGraphQL< TResultType >::GetQuery() const
{
    FString query = GetRawQuery();
    ProcessRawQuery( query );
    return query;
}

template < typename TResultType >
void FGitHubToolsHttpRequestGraphQLQuery< TResultType >::AddParameters( FJsonObject & variables_object ) const
{
    auto * settings = GetDefault< UGitHubToolsSettings >();

    variables_object.SetStringField( TEXT( "repoOwner" ), settings->RepositoryOwner );
    variables_object.SetStringField( TEXT( "repoName" ), settings->RepositoryName );
}

template < typename TResultType >
FGitHubToolsHttpRequestGraphQLQueryWithPagination< TResultType >::FGitHubToolsHttpRequestGraphQLQueryWithPagination( const FString & after_cursor ) :
    AfterCursor( after_cursor ),
    bHasNextPage( false )
{
}

template < typename TResultType >
void FGitHubToolsHttpRequestGraphQLQueryWithPagination< TResultType >::ProcessRawQuery( FString & query ) const
{
    query.ReplaceInline( TEXT( "__CURSOR_INFO__" ), *GetCursorInfo() );
}

template < typename TResultType >
FString FGitHubToolsHttpRequestGraphQLQueryWithPagination< TResultType >::GetCursorInfo() const
{
    TStringBuilder< 128 > string_builder;
    string_builder << TEXT( "first : 100" );

    if ( !AfterCursor.IsEmpty() )
    {
        string_builder << TEXT( ", after: \"" ) << AfterCursor << TEXT( "\"" );
    }

    return *string_builder;
}

template < typename TResultType >
void FGitHubToolsHttpRequestGraphQLQueryWithPagination< TResultType >::ParsePageInfo( const FJsonObject & json_object )
{
    const auto page_info = json_object.GetObjectField( TEXT( "pageInfo" ) );
    EndCursor = page_info->GetStringField( TEXT( "endCursor" ) );
    bHasNextPage = page_info->GetStringField( TEXT( "hasNextPage" ) ) == TEXT( "true" );
}

template < typename TResultType >
void FGitHubToolsHttpRequestRest< TResultType >::ProcessResponse( const FHttpResponsePtr & response_ptr )
{
    TSharedPtr< FJsonValue > data;
    if ( !DeserializeResponseContent( response_ptr, data ) )
    {
        return;
    }

    ParseResponseData( *data.Get() );
}

template < typename TResultType >
FString FGitHubToolsHttpRequestRest< TResultType >::GetVerb() const
{
    static const FString Verb = TEXT( "GET" );
    return Verb;
}

template < typename TResultType >
FString FGitHubToolsHttpRequestRest< TResultType >::GetURL() const
{
    static const FString URL( TEXT( "https://api.github.com/graphql" ) );
    auto * settings = GetDefault< UGitHubToolsSettings >();

    TStringBuilder< 256 > url_string_builder;

    url_string_builder << TEXT( "https://api.github.com/repos/" );
    url_string_builder << settings->RepositoryOwner;
    url_string_builder << TEXT( "/" );
    url_string_builder << settings->RepositoryName;
    url_string_builder << TEXT( "/" );
    url_string_builder << GetEndPoint();

    return *url_string_builder;
}

template < typename TResultType >
FGitHubToolsHttpRequestRestQueryWithPagination< TResultType >::FGitHubToolsHttpRequestRestQueryWithPagination( const int page_index ) :
    PageIndex( page_index ),
    bHasNextPage( false )
{
}

template < typename TResultType >
void FGitHubToolsHttpRequestRestQueryWithPagination< TResultType >::ProcessResponse( const FHttpResponsePtr & response_ptr )
{
    for ( const auto & header : response_ptr->GetAllHeaders() )
    {
        if ( !header.StartsWith( TEXT( "Link" ) ) )
        {
            continue;
        }
        if ( header.Contains( TEXT( "rel=\"next\"" ) ) )
        {
            bHasNextPage = true;
            break;
        }
    }

    FGitHubToolsHttpRequestRestQuery< TResultType >::ProcessResponse( response_ptr );
}

template < typename TResultType >
FString FGitHubToolsHttpRequestRestQueryWithPagination< TResultType >::GetURL() const
{
    TStringBuilder< 256 > url_string_builder;
    url_string_builder.Append( FGitHubToolsHttpRequestRestQuery< TResultType >::GetURL() );
    url_string_builder.Append( FString::Printf( TEXT( "?per_page=100&page=%i" ), PageIndex ) );

    return *url_string_builder;
}