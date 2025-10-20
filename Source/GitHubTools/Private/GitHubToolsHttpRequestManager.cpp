#include "GitHubToolsHttpRequestManager.h"

#include "Async/Async.h"
#include "GitHubTools.h"
#include "GitHubToolsSettings.h"
#include "HttpModule.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

template < typename TRequest >
bool TGitHubToolsHttpRequestWrapper< TRequest >::ProcessRequest()
{
    auto * settings = GetDefault< UGitHubToolsSettings >();
    const auto token = settings->Token;
    const auto http_request = FHttpModule::Get().CreateRequest();

    http_request->SetHeader( TEXT( "Accept" ), TEXT( "application/json" ) );
    http_request->SetHeader( TEXT( "Content-Type" ), TEXT( "application/vnd.github+json" ) );

    TStringBuilder< 128 > token_builder;
    token_builder << TEXT( "Bearer " ) << token;

    http_request->SetHeader( TEXT( "Authorization" ), *token_builder );
    http_request->SetHeader( TEXT( "X-GitHub-Api-Version" ), TEXT( "2022-11-28" ) );

    Request.SetupHttpRequest( http_request.Get() );
    
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