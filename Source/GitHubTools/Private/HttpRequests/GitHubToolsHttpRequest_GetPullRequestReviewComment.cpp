#include "GitHubToolsHttpRequest_GetPullRequestReviewComment.h"

#include "GitHubToolsTypes.h"

#include <Interfaces/IHttpResponse.h>

#define LOCTEXT_NAMESPACE "GitHubTools.Requests"

FGitHubToolsHttpRequestData_GetPullRequestReviewComments::FGitHubToolsHttpRequestData_GetPullRequestReviewComments( int pull_request_number, int review_id ) :
    PullRequestNumber( pull_request_number ),
    ReviewId( review_id )
{
}

EGitHubToolsRequestType FGitHubToolsHttpRequestData_GetPullRequestReviewComments::GetVerb() const
{
    return EGitHubToolsRequestType::GET;
}

FString FGitHubToolsHttpRequestData_GetPullRequestReviewComments::GetEndPoint() const
{
    return FString::Printf( TEXT( "pulls/%i/reviews/%i/comments" ), PullRequestNumber, ReviewId );
}

FText FGitHubToolsHttpRequestData_GetPullRequestReviewComments::GetNotificationText() const
{
    return LOCTEXT( "GetPullRequestReviewComments", "Fetching the comments of the review" );
}

FText FGitHubToolsHttpRequestData_GetPullRequestReviewComments::GetFailureText() const
{
    return LOCTEXT( "GetPullRequestReviewComments", "Error while fetching the comments of the review" );
}

void FGitHubToolsHttpResponseData_GetPullRequestReviewComments::ParseResponse( FHttpResponsePtr response_ptr )
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
        const auto comment_object = comment_infos->AsObject();
        const auto user_infos_object = comment_object->GetObjectField( TEXT( "user" ) );

        //comments.Emplace( MakeShared< FGithubToolsPullRequestComment >( comment_object->GetStringField( TEXT( "path" ) ), user_infos_object->GetStringField( TEXT( "login" ) ), comment_object->GetStringField( TEXT( "updated_at" ) ) ) );
    }

    ReviewComments = comments;
}

#undef LOCTEXT_NAMESPACE