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

    string_builder << TEXT( "{ \"query\" : \"query ($repoOwner: String!, $repoName: String!, $pullNumber: Int!) {" );
    string_builder << TEXT( "  repository(owner: $repoOwner, name: $repoName) {" );
    string_builder << TEXT( "    pullRequest( number : $pullNumber ) {" );
    string_builder << TEXT( "      number" );
    string_builder << TEXT( "      id" );
    string_builder << TEXT( "      title" );
    string_builder << TEXT( "      author {" );
    string_builder << TEXT( "        login" );
    string_builder << TEXT( "      }" );
    string_builder << TEXT( "      baseRefName" );
    string_builder << TEXT( "      bodyText " );
    string_builder << TEXT( "      changedFiles " );
    string_builder << TEXT( "      createdAt " );
    string_builder << TEXT( "      headRefName " );
    string_builder << TEXT( "      isDraft " );
    string_builder << TEXT( "      mergeable " );
    string_builder << TEXT( "      state " );
    string_builder << TEXT( "      url " );
    string_builder << TEXT( "      commits {" );
    string_builder << TEXT( "        totalCount" );
    string_builder << TEXT( "      }" );
    string_builder << TEXT( "      reviewThreads( first : 100 ) {" );
    string_builder << TEXT( "        nodes {" );
    string_builder << TEXT( "          resolvedBy {" );
    string_builder << TEXT( "            login" );
    string_builder << TEXT( "          } " );
    string_builder << TEXT( "          id" );
    string_builder << TEXT( "          isResolved" );
    string_builder << TEXT( "          path" );
    string_builder << TEXT( "          comments( first : 100 ) {" );
    string_builder << TEXT( "            edges {" );
    string_builder << TEXT( "              node {" );
    string_builder << TEXT( "                author {" );
    string_builder << TEXT( "                  login" );
    string_builder << TEXT( "                } " );
    string_builder << TEXT( "                id" );
    string_builder << TEXT( "                body" );
    string_builder << TEXT( "                createdAt" );
    string_builder << TEXT( "              }" );
    string_builder << TEXT( "            }" );
    string_builder << TEXT( "          }" );
    string_builder << TEXT( "        }" );
    string_builder << TEXT( "      }" );
    string_builder << TEXT( "      checks: commits( last: 1 ) {" );
    string_builder << TEXT( "        edges {" );
    string_builder << TEXT( "          node {" );
    string_builder << TEXT( "            commit {" );
    string_builder << TEXT( "              status {" );
    string_builder << TEXT( "                contexts {" );
    string_builder << TEXT( "                  context" );
    string_builder << TEXT( "                  state" );
    string_builder << TEXT( "                  description" );
    string_builder << TEXT( "                }" );
    string_builder << TEXT( "              }" );
    string_builder << TEXT( "            }" );
    string_builder << TEXT( "          }" );
    string_builder << TEXT( "        }" );
    string_builder << TEXT( "      }" );
    string_builder << TEXT( "    }" );
    string_builder << TEXT( "  }" );
    string_builder << TEXT( "}" );
    string_builder << TEXT( "\"," );
    string_builder << TEXT( "\"variables\": " );
    string_builder << TEXT( "  {" );
    string_builder << TEXT( "    \"repoOwner\": \"FishingCactus\"," );
    string_builder << TEXT( "    \"repoName\": \"SummerCamp\"," );
    string_builder << TEXT( "    \"pullNumber\": " << PullRequestNumber );
    string_builder << TEXT( "  }" );
    string_builder << TEXT( "}" );

    return *string_builder;
}

void FGitHubToolsHttpRequestData_GetPullRequestInfos::ParseResponse( FHttpResponsePtr response_ptr )
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

    auto pr_infos = MakeShared< FGithubToolsPullRequestInfos >( pull_request_object );

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
        review_thread_infos->PRNumber = pr_infos->Number;

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

    const auto checks_object = pull_request_object->GetObjectField( TEXT( "checks" ) );
    const auto checks_edges_object = checks_object->GetArrayField( TEXT( "edges" ) );
    const auto check_node_object = checks_edges_object[ 0 ]->AsObject()->GetObjectField( TEXT( "node" ) );
    const auto commit_object = check_node_object->GetObjectField( TEXT( "commit" ) );
    const auto status_json = commit_object->GetObjectField( TEXT( "status" ) );
    const auto contexts_json = status_json->GetArrayField( TEXT( "contexts" ) );

    pr_infos->Checks.Reserve( contexts_json.Num() );

    for ( const auto check_object : contexts_json )
    {
        pr_infos->Checks.Emplace( MakeShared< FGitHubToolsPullRequestCheckInfos >( check_object->AsObject() ) );
    }

    Result = pr_infos;
}

#undef LOCTEXT_NAMESPACE