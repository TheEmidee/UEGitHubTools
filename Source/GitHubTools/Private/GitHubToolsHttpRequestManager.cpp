#include "GitHubToolsHttpRequestManager.h"

#include "GitHubTools.h"
#include "GitHubToolsSettings.h"

#include <HttpModule.h>

template < typename TRequest, typename TResponse >
bool TGitHubToolsHttpRequest< TRequest, TResponse >::ProcessRequest()
{
    FString token;
    FString repository_owner;
    FString repository_name;

    if ( auto * settings = GetDefault< UGitHubToolsSettings >() )
    {
        token = settings->Token;
        repository_owner = settings->RepositoryOwner;
        repository_name = settings->RepositoryName;
    }

    if ( token.IsEmpty() )
    {
        return false;
    }

    if ( repository_owner.IsEmpty() )
    {
        return false;
    }

    if ( repository_name.IsEmpty() )
    {
        return false;
    }

    const auto request = FHttpModule::Get().CreateRequest();

    static const FString verbs[] = {
        TEXT( "GET" ),
        TEXT( "POST" )
    };

    request->SetVerb( verbs[ static_cast< uint8 >( Request.GetVerb() ) ] );
    request->SetHeader( TEXT( "Accept" ), TEXT( "application/json" ) );
    request->SetHeader( TEXT( "Content-Type" ), TEXT( "application/vnd.github+json" ) );

    TStringBuilder< 128 > token_builder;
    token_builder << TEXT( "Bearer " );
    token_builder << token;

    request->SetHeader( TEXT( "Authorization" ), *token_builder );
    request->SetHeader( TEXT( "X-GitHub-Api-Version" ), TEXT( "2022-11-28" ) );

    TStringBuilder< 256 > string_builder;
    string_builder << TEXT( "https://api.github.com/repos/" );
    string_builder << repository_owner;
    string_builder << TEXT( "/" );
    string_builder << repository_name;
    string_builder << TEXT( "/" );
    string_builder << Request.GetEndPoint();

    request->SetURL( *string_builder );
    request->SetContentAsString( Request.GetBody() );
    request->OnProcessRequestComplete().BindRaw( this, &::TGitHubToolsHttpRequest< TRequest, TResponse >::OnProcessRequestComplete );

    return request->ProcessRequest();
}

template < typename TRequest, typename TResponse >
void TGitHubToolsHttpRequest< TRequest, TResponse >::OnProcessRequestComplete( FHttpRequestPtr request_ptr, FHttpResponsePtr response_ptr, bool success )
{
    TResponse response;
    if ( success )
    {
        response.ParseResponse( response_ptr );
        FGitHubToolsModule::Get().GetNotificationManager().RemoveInProgressNotification();
    }
    else
    {
        FGitHubToolsModule::Get().GetNotificationManager().DisplayFailureNotification( Request.GetFailureText() );
    }

    Promise.SetValue( MoveTemp( response ) );
}

template < typename TRequest, typename TResponse, typename... TArgTypes >
TFuture< TResponse > FGitHubToolsHttpRequestManager::SendRequest( TArgTypes &&... args )
{
    static_assert( TIsDerivedFrom< TRequest, FGitHubToolsHttpRequestData >::IsDerived, "Sent RequestType need to derive from FGitHubToolsHttpRequest." );

    typedef TGitHubToolsHttpRequest< TRequest, TResponse > HttpRequestType;
    auto request = MakeShared< HttpRequestType >( Forward< TArgTypes >( args )... );

    Request = request;

    FGitHubToolsModule::Get().GetNotificationManager().DisplayInProgressNotification( request->GetRequestData().GetNotificationText() );

    request->ProcessRequest();
    return request->GetFuture();
}