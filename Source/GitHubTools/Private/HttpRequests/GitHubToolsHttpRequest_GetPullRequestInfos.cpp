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
    add_to_string_builder( TEXT( "  repository(owner: $repoOwner, name: $repoName) {" ) );
    add_to_string_builder( TEXT( "      pullRequest( number : $pullNumber ) {" ) );
    add_to_string_builder( TEXT( "          number" ) );
    add_to_string_builder( TEXT( "          title" ) );
    add_to_string_builder( TEXT( "          files( first : 100 ) {" ) );
    add_to_string_builder( TEXT( "              edges {" ) );
    add_to_string_builder( TEXT( "                  node {" ) );
    add_to_string_builder( TEXT( "                      path" ) );
    add_to_string_builder( TEXT( "                      changeType" ) );
    add_to_string_builder( TEXT( "                  }" ) );
    add_to_string_builder( TEXT( "              }" ) );
    add_to_string_builder( TEXT( "          }" ) );
    add_to_string_builder( TEXT( "          reviewThreads( first : 100 ) {" ) );
    add_to_string_builder( TEXT( "              nodes {" ) );
    add_to_string_builder( TEXT( "                  resolvedBy {" ) );
    add_to_string_builder( TEXT( "                      login" ) );
    add_to_string_builder( TEXT( "                  } " ) );
    add_to_string_builder( TEXT( "                  id" ) );
    add_to_string_builder( TEXT( "                  isResolved" ) );
    add_to_string_builder( TEXT( "                  path" ) );
    add_to_string_builder( TEXT( "                  comments( first : 100 ) {" ) );
    add_to_string_builder( TEXT( "                      edges {" ) );
    add_to_string_builder( TEXT( "                          node {" ) );
    add_to_string_builder( TEXT( "                              author {" ) );
    add_to_string_builder( TEXT( "                                  login" ) );
    add_to_string_builder( TEXT( "                              } " ) );
    add_to_string_builder( TEXT( "                              id" ) );
    add_to_string_builder( TEXT( "                              body" ) );
    add_to_string_builder( TEXT( "                              createdAt" ) );
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
    add_to_string_builder( TEXT( "    \"pullNumber\": 730" ) );
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

    TSharedPtr< FJsonValue > data;
    if ( !FJsonSerializer::Deserialize( json_reader, data ) )
    {
        return;
    }

    const auto data_object = data->AsObject()->GetObjectField( TEXT( "data" ) );
    const auto repository_object = data_object->GetObjectField( TEXT( "repository" ) );
    const auto pull_request_object = repository_object->GetObjectField( TEXT( "pullRequest" ) );

    auto pr_infos = MakeShared< FGithubToolsPullRequestInfos >( pull_request_object->GetIntegerField( TEXT( "number" ) ), pull_request_object->GetStringField( TEXT( "title" ) ) );

    const auto files_object = pull_request_object->GetObjectField( TEXT( "files" ) );
    const auto files_edges_object = files_object->GetArrayField( TEXT( "edges" ) );

    pr_infos->FileInfos.Reserve( files_edges_object.Num() );

    for ( const auto file_object : files_edges_object )
    {
        const auto file_node_object = file_object->AsObject()->GetObjectField( TEXT( "node" ) );

        pr_infos->FileInfos.Emplace( MakeShared< FGithubToolsPullRequestFileInfos >(
            file_node_object->GetStringField( TEXT( "path" ) ),
            file_node_object->GetStringField( TEXT( "changeType" ) ) ) );
    }

    const auto reviews_object = pull_request_object->GetObjectField( TEXT( "reviewThreads" ) );
    const auto reviews_nodes_object = reviews_object->GetArrayField( TEXT( "nodes" ) );

    pr_infos->Reviews.Reserve( reviews_nodes_object.Num() );

    for ( const auto review_object : reviews_nodes_object )
    {
        const auto review_node_object = review_object->AsObject();

        const auto get_resolved_by_user_name = [ &review_node_object ]() {
            FString user_name( TEXT( "" ) );
            if ( review_node_object->HasField( TEXT( "resolvedBy" ) ) )
            {
                const TSharedPtr< FJsonObject > * review_author_object;

                if ( review_node_object->TryGetObjectField( TEXT( "resolvedBy" ), review_author_object ) )
                {
                    user_name = ( *review_author_object )->GetStringField( TEXT( "login" ) );
                }
            }

            return FText::FromString( user_name );
        };

        auto review_thread_infos = MakeShared< FGithubToolsPullRequestReviewThreadInfos >();
        review_thread_infos->Id = review_node_object->GetStringField( TEXT( "id" ) );
        review_thread_infos->bIsResolved = review_node_object->GetBoolField( TEXT( "isResolved" ) );
        review_thread_infos->FileName = review_node_object->GetStringField( TEXT( "path" ) );
        review_thread_infos->ResolvedByUserName = get_resolved_by_user_name();

        const auto comments_object = review_node_object->GetObjectField( TEXT( "comments" ) );
        const auto comments_edges_object = comments_object->GetArrayField( TEXT( "edges" ) );

        review_thread_infos->Comments.Reserve( comments_edges_object.Num() );

        for ( const auto comment_object : comments_edges_object )
        {
            const auto comment_node_object = comment_object->AsObject()->GetObjectField( TEXT( "node" ) );
            const auto comment_author_object = comment_node_object->GetObjectField( TEXT( "author" ) );

            auto comment = MakeShared< FGithubToolsPullRequestComment >();

            comment->Id = comment_node_object->GetStringField( TEXT( "id" ) );
            comment->Author = FText::FromString( comment_author_object->GetStringField( TEXT( "login" ) ) );
            comment->Comment = FText::FromString( comment_node_object->GetStringField( TEXT( "body" ) ) );
            comment->Date = FText::FromString( comment_node_object->GetStringField( TEXT( "createdAt" ) ) );

            review_thread_infos->Comments.Emplace( comment );
        }

        pr_infos->Reviews.Emplace( review_thread_infos );
    }

    PRInfos = pr_infos;
}

#undef LOCTEXT_NAMESPACE