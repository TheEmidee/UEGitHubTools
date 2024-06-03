#pragma once

#include "GitHubToolsSettings.h"
#include "HttpModule.h"

#include <CoreMinimal.h>
#include <Interfaces/IHttpRequest.h>

enum class EGitHubToolsRequestType : uint8
{
    GET,
    POST
};

class FGitHubToolsHttpRequestData
{
public:
    virtual ~FGitHubToolsHttpRequestData() = default;

    virtual EGitHubToolsRequestType GetVerb() const = 0;
    virtual FString GetEndPoint() const = 0;
    virtual FString GetBody() const
    {
        return TEXT( "" );
    }
};

class FGitHubToolsHttpResponseData
{
public:
    virtual ~FGitHubToolsHttpResponseData() = default;

    virtual void ParseResponse( FHttpResponsePtr response_ptr ) = 0;
};

class IGitHubToolsHttpRequest
{
};

template < typename TRequest, typename TResponse >
class TGitHubToolsHttpRequest : public IGitHubToolsHttpRequest
{
public:
    static_assert( TIsDerivedFrom< TRequest, FGitHubToolsHttpRequestData >::IsDerived, "TGitHubToolsHttpRequest TRequest must derive from FGitHubToolsHttpRequestRequestData." );
    static_assert( TIsDerivedFrom< TResponse, FGitHubToolsHttpResponseData >::IsDerived, "TGitHubToolsHttpRequest TResponse must derive from FGitHubToolsHttpRequestResponseData." );

    template < typename... TArgTypes >
    TGitHubToolsHttpRequest( TArgTypes &&... args ) :
        Request( Forward< TArgTypes >( args )... )
    {}

    virtual ~TGitHubToolsHttpRequest() = default;

    bool ProcessRequest();

    FORCEINLINE TFuture< TResponse > GetFuture()
    {
        return Promise.GetFuture();
    }

protected:
    void OnProcessRequestComplete( FHttpRequestPtr request_ptr, FHttpResponsePtr response_ptr, bool success );

    TPromise< TResponse > Promise;
    TRequest Request;
};

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

    request->SetDelegateThreadPolicy( EHttpRequestDelegateThreadPolicy::CompleteOnHttpThread );

    return request->ProcessRequest();
}

template < typename TRequest, typename TResponse >
void TGitHubToolsHttpRequest< TRequest, TResponse >::OnProcessRequestComplete( FHttpRequestPtr request_ptr, FHttpResponsePtr response_ptr, bool success )
{
    TResponse response;
    if ( success )
    {
        response.ParseResponse( response_ptr );
    }
    Promise.SetValue( MoveTemp( response ) );
}

class FGitHubToolsHttpRequestManager : public TSharedFromThis< FGitHubToolsHttpRequestManager >
{
public:
    template < typename TRequest, typename TResponse, typename... TArgTypes >
    TFuture< TResponse > SendRequest( TArgTypes &&... args )
    {
        static_assert( TIsDerivedFrom< TRequest, FGitHubToolsHttpRequestData >::IsDerived, "Sent RequestType need to derive from FGitHubToolsHttpRequest." );

        typedef TGitHubToolsHttpRequest< TRequest, TResponse > HttpRequestType;
        auto request = MakeShared< HttpRequestType >( Forward< TArgTypes >( args )... );

        Request = request;

        request->ProcessRequest();
        return request->GetFuture();
    }

private:
    TSharedPtr< IGitHubToolsHttpRequest > Request;
};

class FGitHubToolsHttpRequestData_GetPullRequestNumber : public FGitHubToolsHttpRequestData
{
public:
    EGitHubToolsRequestType GetVerb() const override
    {
        return EGitHubToolsRequestType::GET;
    }

    FString GetEndPoint() const override
    {
        return TEXT( "pulls" );
    }
};

class FGitHubToolsHttpResponseData_GetPullRequestNumber final : public FGitHubToolsHttpResponseData
{
public:
    FORCEINLINE TOptional< int > GetPullRequestNumber() const
    {
        return PullRequestNumber;
    }

    void ParseResponse( FHttpResponsePtr response_ptr ) override;

private:
    TOptional< int > PullRequestNumber;
};

/*class FGithubToolsRequest_GetPullRequestFiles
{
public:
    TFuture< TArray< FString > > GetPullRequestFiles( int pull_request_number );

private:
    TPromise< TArray< FString > > Promise;
};*/