#include "GitHubToolsHttpRequest_GetPullRequestInfos.h"

#include <Interfaces/IHttpResponse.h>

#define LOCTEXT_NAMESPACE "GitHubTools.Requests"

FGitHubToolsHttpRequestData_GetPullRequestInfos::FGitHubToolsHttpRequestData_GetPullRequestInfos( int pull_request_number ) :
    PullRequestNumber( pull_request_number )
{
}

bool FGitHubToolsHttpRequestData_GetPullRequestInfos::UsesGraphQL() const
{
    return true;
}

FString FGitHubToolsHttpRequestData_GetPullRequestInfos::GetBody() const
{
    TStringBuilder< 512 > string_builder;

    const auto add_to_string_builder = [ &string_builder ]( const FString & string ) {
        auto count = 0;
        auto index = 0;

        while ( string[ index++ ] == ' ' )
        {
            ++count;
        }

        string_builder << string.RightChop( FMath::Max( 0, count - 1 ) );
    };

    add_to_string_builder( TEXT( "{ \"query\" : \"query ($repoOwner: String!, $repoName: String!, $pullNumber: Int!) {" ) );
    add_to_string_builder( TEXT( "repository(owner: $repoOwner, name: $repoName) {" ) );
    add_to_string_builder( TEXT( "      pullRequest( number : $pullNumber )" ) );
    add_to_string_builder( TEXT( "      {" ) );
    add_to_string_builder( TEXT( "          title" ) );
    add_to_string_builder( TEXT( "          comments( first : 100 )" ) );
    add_to_string_builder( TEXT( "          {" ) );
    add_to_string_builder( TEXT( "              edges" ) );
    add_to_string_builder( TEXT( "              {" ) );
    add_to_string_builder( TEXT( "                  node" ) );
    add_to_string_builder( TEXT( "                  {" ) );
    add_to_string_builder( TEXT( "                      author {" ) );
    add_to_string_builder( TEXT( "                          login" ) );
    add_to_string_builder( TEXT( "                      } " ) );
    add_to_string_builder( TEXT( "                      body" ) );
    add_to_string_builder( TEXT( "                      createdAt" ) );
    add_to_string_builder( TEXT( "                  }" ) );
    add_to_string_builder( TEXT( "              }" ) );
    add_to_string_builder( TEXT( "          }" ) );
    add_to_string_builder( TEXT( "          reviews( first : 100 )" ) );
    add_to_string_builder( TEXT( "          {" ) );
    add_to_string_builder( TEXT( "              edges" ) );
    add_to_string_builder( TEXT( "              {" ) );
    add_to_string_builder( TEXT( "                  node" ) );
    add_to_string_builder( TEXT( "                  {" ) );
    add_to_string_builder( TEXT( "                      author {" ) );
    add_to_string_builder( TEXT( "                          login" ) );
    add_to_string_builder( TEXT( "                      } " ) );
    add_to_string_builder( TEXT( "                      id" ) );
    add_to_string_builder( TEXT( "                      body" ) );
    add_to_string_builder( TEXT( "                      state" ) );
    add_to_string_builder( TEXT( "                      submittedAt" ) );
    add_to_string_builder( TEXT( "                      comments( first : 100 )" ) );
    add_to_string_builder( TEXT( "                      {" ) );
    add_to_string_builder( TEXT( "                          edges" ) );
    add_to_string_builder( TEXT( "                          {" ) );
    add_to_string_builder( TEXT( "                              node" ) );
    add_to_string_builder( TEXT( "                              {" ) );
    add_to_string_builder( TEXT( "                                  author {" ) );
    add_to_string_builder( TEXT( "                                      login" ) );
    add_to_string_builder( TEXT( "                                  } " ) );
    add_to_string_builder( TEXT( "                                  id" ) );
    add_to_string_builder( TEXT( "                                  body" ) );
    add_to_string_builder( TEXT( "                                  path" ) );
    add_to_string_builder( TEXT( "                                  position" ) );
    add_to_string_builder( TEXT( "                                  createdAt" ) );
    add_to_string_builder( TEXT( "                              }" ) );
    add_to_string_builder( TEXT( "                          }" ) );
    add_to_string_builder( TEXT( "                      }" ) );
    add_to_string_builder( TEXT( "                  }" ) );
    add_to_string_builder( TEXT( "              }" ) );
    add_to_string_builder( TEXT( "          }" ) );
    add_to_string_builder( TEXT( "      }" ) );
    add_to_string_builder( TEXT( "  }" ) );
    add_to_string_builder( TEXT( "}" ) );
    add_to_string_builder( TEXT( "\"," ) );
    add_to_string_builder( TEXT( "\"variables\": " ) );
    add_to_string_builder( TEXT( "  {" ) );
    add_to_string_builder( TEXT( "    \"repoOwner\": \"FishingCactus\"," ) );
    add_to_string_builder( TEXT( "    \"repoName\": \"SummerCamp\"," ) );
    add_to_string_builder( TEXT( "    \"pullNumber\": 573" ) );
    add_to_string_builder( TEXT( "  }" ) );
    add_to_string_builder( TEXT( "}" ) );

    return *string_builder;
}

FText FGitHubToolsHttpRequestData_GetPullRequestInfos::GetNotificationText() const
{
    return LOCTEXT( "GetPullRequestReviews", "Fetching the reviews of the pull request" );
}

FText FGitHubToolsHttpRequestData_GetPullRequestInfos::GetFailureText() const
{
    return LOCTEXT( "GetPullRequestReviews", "Error while fetching the reviews of the pull request" );
}

void FGitHubToolsHttpResponseData_GetPullRequestInfos::ParseResponse( FHttpResponsePtr response_ptr )
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

        reviews.Emplace( MakeShared< FGithubToolsPullRequestReviewInfos >( review_infos_object->GetIntegerField( TEXT( "id" ) ), user_infos_object->GetStringField( TEXT( "login" ) ), review_infos_object->GetStringField( TEXT( "state" ) ) ) );
    }

    //Reviews = reviews;
}

#undef LOCTEXT_NAMESPACE