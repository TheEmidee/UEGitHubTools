#include "GitHubToolsHttpRequestManager.h"

#include "Async/Async.h"
#include "GitHubTools.h"
#include "GitHubToolsHelpers.h"
#include "GitHubToolsHttpRequestsTypes.h"
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
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 3
    http_request->SetDelegateThreadPolicy( EHttpRequestDelegateThreadPolicy::CompleteOnHttpThread );
#endif

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

#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 3
    return Async( EAsyncExecution::TaskGraph, [... args = Forward< TArgTypes >( args ) ]() {
#else
    auto args_tuple = MakeTuple( Forward< TArgTypes >( args )... );

    return Async( EAsyncExecution::TaskGraph, [ args_tuple = MoveTemp( args_tuple ) ]() mutable {
#endif
        FString cursor;
        int page_index = 1;
        TResult result;

        while ( true )
        {
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 3
            typedef TGitHubToolsHttpRequestWrapper< TRequest > HttpRequestType;
            auto request = MakeShared< HttpRequestType >( args..., [ & ]() {
                if constexpr ( TRequestTraits< TRequest >::IsGraphQL )
                    return cursor;
                else
                    return page_index;
            }() );
#else
            auto request = [ & ]() {
                if constexpr ( TRequestTraits< TRequest >::IsGraphQL )
                    return MakeRequestWithTuple< TRequest >( args_tuple, cursor );
                else
                    return MakeRequestWithTuple< TRequest >( args_tuple, page_index );
            }();
#endif
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

            if constexpr ( TRequestTraits< TRequest >::IsGraphQL )
            {
                cursor = request_future_result.GetEndCursor();
            }
            else if constexpr ( TRequestTraits< TRequest >::IsRest )
            {
                page_index++;
            }
        }
    } );
}