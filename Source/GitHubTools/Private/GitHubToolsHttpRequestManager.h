#pragma once

#include "GitHubToolsTypes.h"

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

    virtual FText GetNotificationText() const = 0;
    virtual FText GetFailureText() const = 0;
};

class FGitHubToolsHttpResponseData
{
public:
    virtual ~FGitHubToolsHttpResponseData() = default;

    virtual void ParseResponse( FHttpResponsePtr response_ptr ) = 0;
};

class IGitHubToolsHttpRequest
{
public:
    virtual ~IGitHubToolsHttpRequest() = default;
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

    virtual ~TGitHubToolsHttpRequest() override = default;

    bool ProcessRequest();

    FORCEINLINE const TRequest & GetRequestData()
    {
        return Request;
    }

    FORCEINLINE TFuture< TResponse > GetFuture()
    {
        return Promise.GetFuture();
    }

protected:
    void OnProcessRequestComplete( FHttpRequestPtr request_ptr, FHttpResponsePtr response_ptr, bool success );

    TPromise< TResponse > Promise;
    TRequest Request;
};

class FGitHubToolsHttpRequestManager : public TSharedFromThis< FGitHubToolsHttpRequestManager >
{
public:
    template < typename TRequest, typename TResponse, typename... TArgTypes >
    TFuture< TResponse > SendRequest( TArgTypes &&... args );

private:
    TSharedPtr< IGitHubToolsHttpRequest > Request;
};