#pragma once

#include "CoreMinimal.h"
#include "HttpFwd.h"

class FJsonValue;
class FJsonObject;
class IHttpRequest;

template < typename TResultType >
class FGitHubToolsHttpRequest
{
public:
    using ResultType = TResultType;

    template < typename >
    friend class TGitHubToolsHttpRequestWrapper;

    virtual ~FGitHubToolsHttpRequest() = default;

    virtual void SetupHttpRequest( IHttpRequest & http_request );

    FORCEINLINE const TOptional< TResultType > & GetResult() const
    {
        return Result;
    }

    bool HasErrorMessage() const
    {
        return !ErrorMessage.IsEmpty();
    }

    const FString & GetErrorMessage() const
    {
        return ErrorMessage;
    }

    virtual void ProcessResponse( const FHttpResponsePtr & response_ptr ) = 0;

protected:
    virtual FString GetVerb() const = 0;
    virtual FString GetURL() const = 0;

    FString ErrorMessage;
    TOptional< TResultType > Result;
};

template < typename TResultType >
class FGitHubToolsHttpRequestGraphQL : public FGitHubToolsHttpRequest< TResultType >
{
public:
    void SetupHttpRequest( IHttpRequest & http_request ) override;
    void ProcessResponse( const FHttpResponsePtr & response_ptr ) override;

protected:
    virtual void ParseResponseData( const FJsonObject & json_data ) = 0;
    virtual void ProcessRawQuery( FString & query ) const;
    virtual FString GetRawQuery() const = 0;
    virtual void AddParameters( FJsonObject & variables_object ) const;
    FString GetVerb() const override;
    FString GetURL() const override;
    FString GetQuery() const;
};

template < typename TResultType >
class FGitHubToolsHttpRequestGraphQLQuery : public FGitHubToolsHttpRequestGraphQL< TResultType >
{
protected:
    void AddParameters( FJsonObject & variables_object ) const override;
};

template < typename TResultType >
class FGitHubToolsHttpRequestGraphQLMutation : public FGitHubToolsHttpRequestGraphQL< TResultType >
{
};

template < typename TResultType >
class FGitHubToolsHttpRequestGraphQLQueryWithPagination : public FGitHubToolsHttpRequestGraphQLQuery< TResultType >
{
public:
    explicit FGitHubToolsHttpRequestGraphQLQueryWithPagination( const FString & after_cursor = TEXT( "" ) );

    bool HasNextPage() const
    {
        return bHasNextPage;
    }

    FString GetEndCursor() const
    {
        return EndCursor;
    }

protected:
    void ProcessRawQuery( FString & query ) const override;
    FString GetCursorInfo() const;
    void ParsePageInfo( const FJsonObject & json_object );

private:
    FString AfterCursor;
    bool bHasNextPage;
    FString EndCursor;
};

template < typename TResultType >
class FGitHubToolsHttpRequestRest : public FGitHubToolsHttpRequest< TResultType >
{
public:
    void ProcessResponse( const FHttpResponsePtr & response_ptr ) override;

protected:
    FString GetVerb() const override;
    FString GetURL() const override;
    virtual FString GetEndPoint() const = 0;
    virtual void ParseResponseData( const FJsonValue & json_data ) = 0;
};

template < typename TResultType >
class FGitHubToolsHttpRequestRestQuery : public FGitHubToolsHttpRequestRest< TResultType >
{
};

template < typename TResultType >
class FGitHubToolsHttpRequestRestQueryWithPagination : public FGitHubToolsHttpRequestRestQuery< TResultType >
{
public:
    explicit FGitHubToolsHttpRequestRestQueryWithPagination( const int page_index = 1 );

    bool HasNextPage() const
    {
        return bHasNextPage;
    }

    void ProcessResponse( const FHttpResponsePtr & response_ptr ) override;

protected:
    FString GetURL() const override;

private:
    int PageIndex;
    bool bHasNextPage;
};

template < typename T >
struct TRequestTraits
{
    static constexpr bool IsGraphQL = TIsDerivedFrom< T, FGitHubToolsHttpRequestGraphQL< typename T::ResponseType > >::IsDerived;
    static constexpr bool IsRest = TIsDerivedFrom< T, FGitHubToolsHttpRequestRest< typename T::ResponseType > >::IsDerived;
};