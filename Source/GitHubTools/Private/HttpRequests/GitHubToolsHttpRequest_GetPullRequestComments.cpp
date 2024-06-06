#include "GitHubToolsHttpRequest_GetPullRequestComments.h"

#include <Interfaces/IHttpResponse.h>

#define LOCTEXT_NAMESPACE "GitHubTools.Requests"

FGitHubToolsHttpRequestData_GetPullRequestComments::FGitHubToolsHttpRequestData_GetPullRequestComments( int pull_request_number ) :
    PullRequestNumber( pull_request_number )
{
}

EGitHubToolsRequestType FGitHubToolsHttpRequestData_GetPullRequestComments::GetVerb() const
{
    return EGitHubToolsRequestType::GET;
}

FString FGitHubToolsHttpRequestData_GetPullRequestComments::GetEndPoint() const
{
    return FString::Printf( TEXT( "pulls/%i/comments" ), PullRequestNumber );
}

FText FGitHubToolsHttpRequestData_GetPullRequestComments::GetNotificationText() const
{
    return LOCTEXT( "GetPullRequestNumber_GetPullRequestComments", "Fetching the comments on files in the pull request" );
}

FText FGitHubToolsHttpRequestData_GetPullRequestComments::GetFailureText() const
{
    return LOCTEXT( "GetPullRequestNumber_GetPullRequestComments", "Error while fetching the comments on files in the pull request" );
}

void FGitHubToolsHttpResponseData_GetPullRequestComments::ParseResponse( FHttpResponsePtr response_ptr )
{
    const auto json_response = response_ptr->GetContentAsString();
    const auto json_reader = TJsonReaderFactory<>::Create( json_response );

    TArray< TSharedPtr< FJsonValue > > comments_infos;
    if ( !FJsonSerializer::Deserialize( json_reader, comments_infos ) )
    {
        return;
    }

    TArray< FGithubToolsPullRequestCommentPtr > comments;
    comments.Reserve( comments_infos.Num() );

    for ( const auto comment_infos : comments_infos )
    {
        const auto comment_infos_object = comment_infos->AsObject();
        const auto user_infos_object = comment_infos_object->GetObjectField( TEXT( "user" ) );

        comments.Emplace( MakeShared< FGithubToolsPullRequestComment >( comment_infos_object->GetStringField( TEXT( "path" ) ), user_infos_object->GetStringField( TEXT( "login" ) ), comment_infos_object->GetStringField( TEXT( "updated_at" ) ) ) );
    }

    Comments = comments;
}

#undef LOCTEXT_NAMESPACE