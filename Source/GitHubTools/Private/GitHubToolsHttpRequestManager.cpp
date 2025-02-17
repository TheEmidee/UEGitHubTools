#include "GitHubToolsHttpRequestManager.h"

#include "GitHubTools.h"
#include "GitHubToolsSettings.h"

#include <HttpModule.h>
#include <Interfaces/IHttpResponse.h>

template < typename TRequest >
bool TGitHubToolsHttpRequestWrapper< TRequest >::ProcessRequest()
{
    auto * settings = GetDefault< UGitHubToolsSettings >();
    const auto token = settings->Token;
    const auto request = FHttpModule::Get().CreateRequest();

    const auto verb = Request.UsesGraphQL() ? TEXT( "POST" ) : TEXT( "GET" );

    request->SetVerb( verb );
    request->SetHeader( TEXT( "Accept" ), TEXT( "application/json" ) );
    request->SetHeader( TEXT( "Content-Type" ), TEXT( "application/vnd.github+json" ) );

    TStringBuilder< 128 > token_builder;
    token_builder << TEXT( "Bearer " );
    token_builder << token;

    request->SetHeader( TEXT( "Authorization" ), *token_builder );
    request->SetHeader( TEXT( "X-GitHub-Api-Version" ), TEXT( "2022-11-28" ) );

    TStringBuilder< 256 > url_string_builder;

    if ( Request.UsesGraphQL() )
    {
        url_string_builder << TEXT( "https://api.github.com/graphql" );
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

    request->SetURL( *url_string_builder );
    request->SetContentAsString( Request.GetBody() );
    request->OnProcessRequestComplete().BindRaw( this, &::TGitHubToolsHttpRequestWrapper< TRequest >::OnProcessRequestComplete );
    request->SetDelegateThreadPolicy( EHttpRequestDelegateThreadPolicy::CompleteOnHttpThread );

    return request->ProcessRequest();
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
FGitHubToolsHttpRequestWithPagination< TResultType >::FGitHubToolsHttpRequestWithPagination( const FString & after_cursor ) :
    AfterCursor( after_cursor ),
    bHasNextPage( false )
{
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
FString FGitHubToolsHttpRequestWithPagination< TResultType >::GetPageInfoJson() const
{
    TStringBuilder< 128 > string_builder;

    string_builder << TEXT( "pageInfo { " );
    string_builder << TEXT( "  endCursor " );
    string_builder << TEXT( "  hasNextPage " );
    string_builder << TEXT( "}" );

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