#include "GitHubToolsHttpRequestManager.h"

#include "Async/Async.h"
#include "GitHubTools.h"
#include "GitHubToolsSettings.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

template < typename TRequest >
bool TGitHubToolsHttpRequestWrapper< TRequest >::ProcessRequest()
{
    auto * settings = GetDefault< UGitHubToolsSettings >();
    const auto token = settings->Token;
    const auto http_request = FHttpModule::Get().CreateRequest();

    const auto verb = Request.UsesGraphQL() ? TEXT( "POST" ) : TEXT( "GET" );

    http_request->SetVerb( verb );
    http_request->SetHeader( TEXT( "Accept" ), TEXT( "application/json" ) );
    http_request->SetHeader( TEXT( "Content-Type" ), TEXT( "application/vnd.github+json" ) );

    TStringBuilder< 128 > token_builder;
    token_builder << TEXT( "Bearer " );
    token_builder << token;

    http_request->SetHeader( TEXT( "Authorization" ), *token_builder );
    http_request->SetHeader( TEXT( "X-GitHub-Api-Version" ), TEXT( "2022-11-28" ) );

    TStringBuilder< 256 > url_string_builder;

    if ( Request.UsesGraphQL() )
    {
        url_string_builder << TEXT( "https://api.github.com/graphql" );

        TSharedPtr< FJsonObject > body_object = MakeShared< FJsonObject >();
        body_object->SetStringField( TEXT( "query" ), Request.GetQuery() );

        TSharedPtr< FJsonObject > variables_object = MakeShared< FJsonObject >();
        variables_object->SetStringField( TEXT( "repoOwner" ), settings->RepositoryOwner );
        variables_object->SetStringField( TEXT( "repoName" ), settings->RepositoryName );
        Request.AddParameters( variables_object );
    
        body_object->SetObjectField( TEXT( "variables" ), variables_object );

        FString body;
        TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create( &body );
        if ( !FJsonSerializer::Serialize( body_object.ToSharedRef(), Writer ) )
        {
            return false;
        }

        http_request->SetContentAsString( body );
    }
    else
    {
        url_string_builder << TEXT( "https://api.github.com/repos/" );
        url_string_builder << settings->RepositoryOwner;
        url_string_builder << TEXT( "/" );
        url_string_builder << settings->RepositoryName;
        url_string_builder << TEXT( "/" );
        url_string_builder << Request.GetEndPoint();
    }

    http_request->SetURL( *url_string_builder );
    
    http_request->OnProcessRequestComplete().BindRaw( this, &::TGitHubToolsHttpRequestWrapper< TRequest >::OnProcessRequestComplete );
    http_request->SetDelegateThreadPolicy( EHttpRequestDelegateThreadPolicy::CompleteOnHttpThread );

    return http_request->ProcessRequest();
}

template < typename TRequest >
void TGitHubToolsHttpRequestWrapper< TRequest >::OnProcessRequestComplete( FHttpRequestPtr /*request_ptr*/, FHttpResponsePtr response_ptr, bool success )
{
    if ( success )
    {
        Request.ProcessResponse( response_ptr );
    }

    SetPromiseValue();
}

template < typename TRequest >
void TGitHubToolsHttpRequestWrapper< TRequest >::SetPromiseValue()
{
    if ( bSetPromiseValueOnMainThread )
    {
        AsyncTask( ENamedThreads::GameThread, [ & ]() {
            if ( Request.HasErrorMessage() )
            {
                FGitHubToolsModule::Get()
                    .GetNotificationManager()
                    .DisplayFailureNotification(
                        FText::FromString( FString::Printf( TEXT( "Error with the request : %s" ), *Request.GetErrorMessage() ) ) );
            }

            Promise.SetValue( Request );
        } );
    }
    else
    {
        Promise.SetValue( Request );
    }
}

template < typename TResultType >
FString FGitHubToolsHttpRequest< TResultType >::GetQuery() const
{
    FString query = GetRawQuery();
    ProcessRawQuery( query );
    return query;
}

template < typename TResultType >
void FGitHubToolsHttpRequest< TResultType >::ProcessResponse( const FHttpResponsePtr & response_ptr )
{
    const auto json_response = response_ptr->GetContentAsString();
    const auto json_reader = TJsonReaderFactory<>::Create( json_response );

    TSharedPtr< FJsonObject > result;
    if ( FJsonSerializer::Deserialize( json_reader, result ) )
    {
        const TArray< TSharedPtr< FJsonValue > > * errors;
        if ( result->TryGetArrayField( TEXT( "errors" ), errors ) )
        {
            if ( !errors->IsEmpty() )
            {
                ErrorMessage = ( *errors )[ 0 ]->AsObject()->GetStringField( TEXT( "message" ) );
            }
        }

        if ( ErrorMessage.IsEmpty() )
        {
            result->TryGetStringField( TEXT( "message" ), ErrorMessage );
        }
    }

    if ( !ErrorMessage.IsEmpty() )
    {
        return;
    }

    ParseResponse( response_ptr );
}

template < typename TResultType >
void FGitHubToolsHttpRequest< TResultType >::AddParameters( TSharedPtr< FJsonObject > & variables_object ) const
{
}

template < typename TResultType >
void FGitHubToolsHttpRequest< TResultType >::ProcessRawQuery( FString & query ) const
{
}

template < typename TResultType >
FGitHubToolsHttpRequestWithPagination< TResultType >::FGitHubToolsHttpRequestWithPagination( const FString & after_cursor ) :
    AfterCursor( after_cursor ),
    bHasNextPage( false )
{
}

template < typename TResultType >
void FGitHubToolsHttpRequestWithPagination< TResultType >::ProcessRawQuery( FString & query ) const
{
    query.ReplaceInline( TEXT( "__CURSOR_INFO__" ), *GetCursorInfo() );
}

template < typename TResultType >
FString FGitHubToolsHttpRequestWithPagination< TResultType >::GetCursorInfo() const
{
    TStringBuilder< 128 > string_builder;
    string_builder << TEXT( "first : 100" );

    if ( !AfterCursor.IsEmpty() )
    {
        string_builder << TEXT( ", after: \\\"" ) << AfterCursor << TEXT( "\\\"" );
    }

    return *string_builder;
}

template < typename TResultType >
void FGitHubToolsHttpRequestWithPagination< TResultType >::ParsePageInfo( const TSharedPtr< FJsonObject > & json_object )
{
    const auto page_info = json_object->GetObjectField( TEXT( "pageInfo" ) );
    EndCursor = page_info->GetStringField( TEXT( "endCursor" ) );
    bHasNextPage = page_info->GetStringField( TEXT( "hasNextPage" ) ) == TEXT( "true" );
}

template < typename TRequest, typename... TArgTypes >
TFuture< TRequest > FGitHubToolsHttpRequestManager::SendRequest( TArgTypes &&... args )
{
    typedef TGitHubToolsHttpRequestWrapper< TRequest > HttpRequestType;
    auto request = MakeShared< HttpRequestType >( Forward< TArgTypes >( args )... );

    Request = request;

    Async( EAsyncExecution::TaskGraph, [ &, r = request ]() {
        r->ProcessRequest();
    } );

    return request->GetFuture();
}

template < typename TRequest, typename... TArgTypes >
TFuture< typename TRequest::ResponseType > FGitHubToolsHttpRequestManager::SendPaginatedRequest( TArgTypes &&... args )
{
    typedef typename TRequest::ResponseType TResult;

    return Async( EAsyncExecution::TaskGraph, [... args = Forward< TArgTypes >( args ) ]() {
        FString cursor;
        TResult result;

        while ( true )
        {
            typedef TGitHubToolsHttpRequestWrapper< TRequest > HttpRequestType;
            auto request = MakeShared< HttpRequestType >( args..., cursor );
            request->SetPromiseValueOnHttpThread();
            request->ProcessRequest();

            auto request_future_result = request->GetFuture().Get();

            const auto optional_result = request_future_result.GetResult();

            if ( !optional_result.IsSet() )
            {
                return result;
            }

            result.Append( optional_result.GetValue() );

            if ( !request_future_result.HasNextPage() )
            {
                return result;
            }

            cursor = request_future_result.GetEndCursor();
        }
    } );
}