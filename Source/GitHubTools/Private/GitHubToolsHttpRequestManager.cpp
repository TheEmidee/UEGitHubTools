#include "GitHubToolsHttpRequestManager.h"

#include "GitHubTools.h"
#include "GitHubToolsSettings.h"
#include "Interfaces/IHttpResponse.h"

#include <HttpModule.h>

template < typename TRequest >
bool TGitHubToolsHttpRequest< TRequest >::ProcessRequest()
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

    request->SetVerb( TEXT( "POST" ) );
    request->SetHeader( TEXT( "Accept" ), TEXT( "application/json" ) );
    request->SetHeader( TEXT( "Content-Type" ), TEXT( "application/vnd.github+json" ) );

    TStringBuilder< 128 > token_builder;
    token_builder << TEXT( "Bearer " );
    token_builder << token;

    request->SetHeader( TEXT( "Authorization" ), *token_builder );
    request->SetHeader( TEXT( "X-GitHub-Api-Version" ), TEXT( "2022-11-28" ) );

    request->SetURL( TEXT( "https://api.github.com/graphql" ) );
    request->SetContentAsString( Request.GetBody() );
    request->OnProcessRequestComplete().BindRaw( this, &::TGitHubToolsHttpRequest< TRequest >::OnProcessRequestComplete );
    request->SetDelegateThreadPolicy( EHttpRequestDelegateThreadPolicy::CompleteOnHttpThread );

    return request->ProcessRequest();
}

template < typename TRequest >
void TGitHubToolsHttpRequest< TRequest >::OnProcessRequestComplete( FHttpRequestPtr request_ptr, FHttpResponsePtr response_ptr, bool success )
{
    if ( success )
    {
        Request.ProcessResponse( response_ptr );
    }

    AsyncTask( ENamedThreads::GameThread, [ & ]() {
        if ( Request.HasErrorMessage() )
        {
            FGitHubToolsModule::Get().GetNotificationManager().DisplayFailureNotification( FText::FromString( Request.GetErrorMessage() ) );
        }

        Promise.SetValue( Request );
    } );
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
    typedef TGitHubToolsHttpRequest< TRequest > HttpRequestType;
    auto request = MakeShared< HttpRequestType >( Forward< TArgTypes >( args )... );

    Request = request;

    //auto future = t->promise.GetFuture();

    Async( EAsyncExecution::TaskGraph, [ &, r = request ]() {
        //FPlatformProcess::Sleep( 3.0f );
        r->ProcessRequest();
    } );

    return request->GetFuture();
}