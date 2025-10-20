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