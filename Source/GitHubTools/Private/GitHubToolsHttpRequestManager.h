#pragma once

#include "Async/Future.h"
#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"

class FJsonObject;

class IGitHubToolsHttpRequest
{
public:
    virtual ~IGitHubToolsHttpRequest() = default;
};

template < typename TRequest >
class TGitHubToolsHttpRequestWrapper final : public IGitHubToolsHttpRequest
{
public:
    template < typename... TArgTypes >
    TGitHubToolsHttpRequestWrapper( TArgTypes &&... args ) :
        Request( Forward< TArgTypes >( args )... )
    {}

    virtual ~TGitHubToolsHttpRequestWrapper() override = default;

    FORCEINLINE const TRequest & GetRequestData()
    {
        return Request;
    }

    FORCEINLINE TFuture< TRequest > GetFuture()
    {
        return Promise.GetFuture();
    }

    FORCEINLINE void SetPromiseValueOnHttpThread()
    {
        bSetPromiseValueOnMainThread = false;
    }

    bool ProcessRequest();

protected:
    void OnProcessRequestComplete( FHttpRequestPtr request_ptr, FHttpResponsePtr response_ptr, bool success );
    void SetPromiseValue();

    TPromise< TRequest > Promise;
    TRequest Request;
    bool bSetPromiseValueOnMainThread = true;
};

template < typename TResultType >
class FGitHubToolsHttpRequest
{
public:
    using ResultType = TResultType;

    template < typename >
    friend class TGitHubToolsHttpRequestWrapper;

    virtual ~FGitHubToolsHttpRequest() = default;

    virtual void SetupHttpRequest( TSharedRef< IHttpRequest > http_request );

    FORCEINLINE const TOptional< TResultType > & GetResult() const
    {
        return Result;
    }

    FString GetQuery() const;

    bool HasErrorMessage() const
    {
        return !ErrorMessage.IsEmpty();
    }

    const FString & GetErrorMessage() const
    {
        return ErrorMessage;
    }

    void ProcessResponse( const FHttpResponsePtr & response_ptr );
    virtual void AddParameters( TSharedPtr< FJsonObject > & variables_object ) const;

protected:
    virtual FString GetVerb() const = 0; 
    virtual FString GetURL() const = 0;
    virtual void ProcessRawQuery( FString & query ) const;
    virtual FString GetRawQuery() const = 0;
    virtual void ParseResponse( FHttpResponsePtr response_ptr ) = 0;

    FString ErrorMessage;
    TOptional< TResultType > Result;
};

template < typename TResultType >
class FGitHubToolsHttpRequestGraphQL : public FGitHubToolsHttpRequest< TResultType >
{
public:
    void SetupHttpRequest( TSharedRef< IHttpRequest > http_request ) override;

protected:
    virtual void SetupBodyJSON( TSharedRef< FJsonObject > body_object ) const = 0;
    FString GetVerb() const override;
    FString GetURL() const override;
};

template < typename TResultType >
class FGitHubToolsHttpRequestGraphQLQuery : public FGitHubToolsHttpRequestGraphQL< TResultType >
{
public:
    void SetupHttpRequest( TSharedRef< IHttpRequest > http_request ) override;

protected:
    virtual void SetupBodyJSON( TSharedRef< FJsonObject > body_object ) const = 0;
    FString GetVerb() const override;
    FString GetURL() const override;
};

template < typename TResultType >
class FGitHubToolsHttpRequestGraphQLMutation : public FGitHubToolsHttpRequestGraphQL< TResultType >
{
public:
    void SetupHttpRequest( TSharedRef< IHttpRequest > http_request ) override;

protected:
    virtual void SetupBodyJSON( TSharedRef< FJsonObject > body_object ) const = 0;
    FString GetVerb() const override;
    FString GetURL() const override;
};

template < typename TResultType >
class FGitHubToolsHttpRequestGraphQLQueryWithPagination : public FGitHubToolsHttpRequestGraphQLQuery< TResultType >
{
public:
    explicit FGitHubToolsHttpRequestGraphQLQueryWithPagination( const FString & after_cursor = TEXT( "" ) );

    FORCEINLINE bool HasNextPage() const
    {
        return bHasNextPage;
    }

    FORCEINLINE FString GetEndCursor() const
    {
        return EndCursor;
    }

protected:
    void ProcessRawQuery( FString & query ) const override;
    FString GetCursorInfo() const;
    void ParsePageInfo( const TSharedPtr< FJsonObject > & json_object );

private:
    FString AfterCursor;
    bool bHasNextPage;
    FString EndCursor;
};

template < typename TResultType >
class FGitHubToolsHttpRequestRest : public FGitHubToolsHttpRequest< TResultType >
{
protected:
    FString GetVerb() const override;
    FString GetURL() const override;
    virtual FString GetEndPoint() const = 0;
};

class FGitHubToolsHttpRequestManager final : public TSharedFromThis< FGitHubToolsHttpRequestManager >
{
public:
    template < typename TRequest, typename... TArgTypes >
    TFuture< TRequest > SendRequest( TArgTypes &&... args );

    template < typename TRequest, typename... TArgTypes >
    TFuture< typename TRequest::ResponseType > SendPaginatedRequest( TArgTypes &&... args );

private:
    TSharedPtr< IGitHubToolsHttpRequest > Request;
};