#pragma once

#include <CoreMinimal.h>
#include <Interfaces/IHttpRequest.h>

enum class EGitHubToolsRequestType : uint8
{
    GET,
    POST
};

template < typename TResultType >
class FGitHubToolsHttpRequest
{
public:
    using ResultType = TResultType;

    virtual ~FGitHubToolsHttpRequest() = default;

    FORCEINLINE const TOptional< TResultType > & GetResult() const
    {
        return Result;
    }

    virtual FString GetBody() const
    {
        return TEXT( "" );
    }

    bool HasErrorMessage() const
    {
        return !ErrorMessage.IsEmpty();
    }

    const FString & GetErrorMessage() const
    {
        return ErrorMessage;
    }

    void ProcessResponse( const FHttpResponsePtr & response_ptr );

protected:
    virtual void ParseResponse( FHttpResponsePtr response_ptr ) = 0;
    FString ErrorMessage;
    TOptional< TResultType > Result;
};

template < typename TResultType >
class FGitHubToolsHttpRequestWithPagination : public FGitHubToolsHttpRequest< TResultType >
{
public:
    explicit FGitHubToolsHttpRequestWithPagination( const FString & after_cursor = TEXT( "" ) );

    FORCEINLINE bool HasNextPage() const
    {
        return bHasNextPage;
    }

    FORCEINLINE FString GetEndCursor() const
    {
        return EndCursor;
    }

protected:
    FString GetCursorInfo() const;
    FString GetPageInfoJson() const;
    void ParsePageInfo( const TSharedPtr< FJsonObject > & json_object );

private:
    FString AfterCursor;
    bool bHasNextPage;
    FString EndCursor;
};

class IGitHubToolsHttpRequest
{
public:
    virtual ~IGitHubToolsHttpRequest() = default;
};

template < typename TRequest >
class TGitHubToolsHttpRequest : public IGitHubToolsHttpRequest
{
public:
    template < typename... TArgTypes >
    TGitHubToolsHttpRequest( TArgTypes &&... args ) :
        Request( Forward< TArgTypes >( args )... )
    {}

    virtual ~TGitHubToolsHttpRequest() override = default;

    bool ProcessRequest();

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

protected:
    void OnProcessRequestComplete( FHttpRequestPtr request_ptr, FHttpResponsePtr response_ptr, bool success );

    TPromise< TRequest > Promise;
    TRequest Request;
    bool bSetPromiseValueOnMainThread = true;
};

class FGitHubToolsHttpRequestManager : public TSharedFromThis< FGitHubToolsHttpRequestManager >
{
public:
    template < typename TRequest, typename... TArgTypes >
    TFuture< TRequest > SendRequest( TArgTypes &&... args );

    template < typename TRequest, typename... TArgTypes >
    TFuture< typename TRequest::ResponseType > SendPaginatedRequest( TArgTypes &&... args );

private:
    TSharedPtr< IGitHubToolsHttpRequest > Request;
};