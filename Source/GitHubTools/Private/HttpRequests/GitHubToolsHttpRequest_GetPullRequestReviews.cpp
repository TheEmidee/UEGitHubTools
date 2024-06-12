#include "GitHubToolsHttpRequest_GetPullRequestReviews.h"

#include <Interfaces/IHttpResponse.h>

#define LOCTEXT_NAMESPACE "GitHubTools.Requests"

FGitHubToolsHttpRequestData_GetPullRequestReviews::FGitHubToolsHttpRequestData_GetPullRequestReviews( int pull_request_number ) :
    PullRequestNumber( pull_request_number )
{
}

EGitHubToolsRequestType FGitHubToolsHttpRequestData_GetPullRequestReviews::GetVerb() const
{
    return EGitHubToolsRequestType::GET;
}

FString FGitHubToolsHttpRequestData_GetPullRequestReviews::GetEndPoint() const
{
    return FString::Printf( TEXT( "pulls/%i/reviews" ), PullRequestNumber );
}

FText FGitHubToolsHttpRequestData_GetPullRequestReviews::GetNotificationText() const
{
    return LOCTEXT( "GetPullRequestReviews", "Fetching the reviews of the pull request" );
}

FText FGitHubToolsHttpRequestData_GetPullRequestReviews::GetFailureText() const
{
    return LOCTEXT( "GetPullRequestReviews", "Error while fetching the reviews of the pull request" );
}

void FGitHubToolsHttpResponseData_GetPullRequestReviews::ParseResponse( FHttpResponsePtr response_ptr )
{
    const auto json_response = response_ptr->GetContentAsString();
    const auto json_reader = TJsonReaderFactory<>::Create( json_response );

    TArray< TSharedPtr< FJsonValue > > reviews_infos;
    if ( !FJsonSerializer::Deserialize( json_reader, reviews_infos ) )
    {
        return;
    }

    TArray< FGithubToolsPullRequestReviewInfosPtr > reviews;
    reviews.Reserve( reviews_infos.Num() );

    for ( const auto review_infos : reviews_infos )
    {
        const auto review_infos_object = review_infos->AsObject();
        const auto user_infos_object = review_infos_object->GetObjectField( TEXT( "user" ) );

        //reviews.Emplace( MakeShared< FGithubToolsPullRequestReviewInfos >( review_infos_object->GetIntegerField( TEXT( "id" ) ), user_infos_object->GetStringField( TEXT( "login" ) ), review_infos_object->GetStringField( TEXT( "state" ) ) ) );
    }

    Reviews = reviews;
}

#undef LOCTEXT_NAMESPACE