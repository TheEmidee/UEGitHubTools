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

    virtual EGitHubToolsRequestType GetVerb() const
    {
        return EGitHubToolsRequestType::GET;
    };

    virtual FString GetEndPoint() const
    {
        return TEXT( "" );
    }

    virtual FString GetBody() const
    {
        return TEXT( "" );
    }

    virtual bool UsesGraphQL() const
    {
        return false;
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

protected:
    void OnProcessRequestComplete( FHttpRequestPtr request_ptr, FHttpResponsePtr response_ptr, bool success );

    TPromise< TRequest > Promise;
    TRequest Request;
};

class FGitHubToolsHttpRequestManager : public TSharedFromThis< FGitHubToolsHttpRequestManager >
{
public:
    template < typename TRequest, typename... TArgTypes >
    TFuture< TRequest > SendRequest( TArgTypes &&... args );

private:
    TSharedPtr< IGitHubToolsHttpRequest > Request;
};