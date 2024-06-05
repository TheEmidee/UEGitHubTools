#include "GitHubToolsHttpRequest.h"

#include "GitHubTools.h"
#include "GitHubToolsSettings.h"
#include "GitSourceControlModule.h"

#include <HttpModule.h>
#include <Interfaces/IHttpResponse.h>

#define LOCTEXT_NAMESPACE "GitHubTools.Requests"

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

EGitHubToolsRequestType FGitHubToolsHttpRequestData_GetPullRequestNumber::GetVerb() const
{
    return EGitHubToolsRequestType::GET;
}

FString FGitHubToolsHttpRequestData_GetPullRequestNumber::GetEndPoint() const
{
    return TEXT( "pulls" );
}

FText FGitHubToolsHttpRequestData_GetPullRequestNumber::GetNotificationText() const
{
    return LOCTEXT( "GetPullRequestNumber_Notification", "Fetching the pull request number" );
}

FText FGitHubToolsHttpRequestData_GetPullRequestNumber::GetFailureText() const
{
    return LOCTEXT( "GetPullRequestNumber+Failure", "Error while fetching the pull request number" );
}

void FGitHubToolsHttpResponseData_GetPullRequestNumber::ParseResponse( FHttpResponsePtr response_ptr )
{
    const auto local_branch_name = FGitSourceControlModule::Get().GetProvider().GetBranchName();
    const auto json_response = response_ptr->GetContentAsString();
    const auto json_reader = TJsonReaderFactory<>::Create( json_response );

    TArray< TSharedPtr< FJsonValue > > pull_requests_infos;
    if ( !FJsonSerializer::Deserialize( json_reader, pull_requests_infos ) )
    {
        return;
    }

    for ( const auto pull_request_infos : pull_requests_infos )
    {
        const auto pr_object = pull_request_infos->AsObject();
        const int number = pr_object->GetIntegerField( TEXT( "number" ) );
        const auto head_object = pr_object->GetObjectField( TEXT( "head" ) );
        const auto ref = head_object->GetStringField( TEXT( "ref" ) );

        if ( ref == local_branch_name )
        {
            PullRequestNumber = number;
            return;
        }
    }

    PullRequestNumber = INDEX_NONE;
}

FGitHubToolsHttpRequestData_GetPullRequestFiles::FGitHubToolsHttpRequestData_GetPullRequestFiles( int pull_request_number ) :
    PullRequestNumber( pull_request_number )
{
}

EGitHubToolsRequestType FGitHubToolsHttpRequestData_GetPullRequestFiles::GetVerb() const
{
    return EGitHubToolsRequestType::GET;
}

FString FGitHubToolsHttpRequestData_GetPullRequestFiles::GetEndPoint() const
{
    return FString::Printf( TEXT( "pulls/%i/files" ), PullRequestNumber );
}

FText FGitHubToolsHttpRequestData_GetPullRequestFiles::GetNotificationText() const
{
    return LOCTEXT( "GetPullRequestNumber_GetPullRequestFiles", "Fetching the files updated in the pull request" );
}

FText FGitHubToolsHttpRequestData_GetPullRequestFiles::GetFailureText() const
{
    return LOCTEXT( "GetPullRequestNumber_GetPullRequestFiles", "Error while fetching the files updated in the pull request" );
}

void FGitHubToolsHttpResponseData_GetPullRequestFiles::ParseResponse( FHttpResponsePtr response_ptr )
{
    const auto json_response = response_ptr->GetContentAsString();
    const auto json_reader = TJsonReaderFactory<>::Create( json_response );

    TArray< TSharedPtr< FJsonValue > > files_infos;
    if ( !FJsonSerializer::Deserialize( json_reader, files_infos ) )
    {
        return;
    }

    TArray< FGithubToolsPullRequestFileInfosPtr > filenames;
    filenames.Reserve( files_infos.Num() );

    for ( const auto file_infos : files_infos )
    {
        const auto file_infos_object = file_infos->AsObject();
        filenames.Emplace( MakeShared< FGithubToolsPullRequestFileInfos >( file_infos_object->GetStringField( TEXT( "filename" ) ), file_infos_object->GetStringField( TEXT( "status" ) ) ) );
    }

    Files = filenames;
}

#undef LOCTEXT_NAMESPACE