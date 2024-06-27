#pragma once

#include <CoreMinimal.h>
#include <Interfaces/IHttpRequest.h>

class IGitHubToolsHttpRequest
{
public:
    virtual ~IGitHubToolsHttpRequest() = default;
};

template < typename TRequest >
class TGitHubToolsHttpRequestWrapper : public IGitHubToolsHttpRequest
{
public:
    template < typename... TArgTypes >
    TGitHubToolsHttpRequestWrapper( TArgTypes &&... args ) :
        Request( Forward< TArgTypes >( args )... )
    {}

    virtual ~TGitHubToolsHttpRequestWrapper() override = default;

    void TryProcessRequest();

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
    bool ProcessRequest();
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

    template < typename > friend class TGitHubToolsHttpRequestWrapper;

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