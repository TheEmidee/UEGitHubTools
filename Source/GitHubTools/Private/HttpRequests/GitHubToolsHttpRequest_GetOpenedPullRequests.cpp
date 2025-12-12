#include "GitHubToolsHttpRequest_GetOpenedPullRequests.h"

#include "Dom/JsonValue.h"
#include "GitHubToolsGitUtils.h"
#include "Interfaces/IHttpResponse.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

#define LOCTEXT_NAMESPACE "GitHubTools.Requests"

FString FGitHubToolsHttpRequest_GetOpenedPullRequests::GetRawQuery() const
{
    return R"(
query ( $repoOwner: String!, $repoName: String! ) {
  repository( owner: $repoOwner, name: $repoName) {
    pullRequests( last: 100, states: OPEN, orderBy: {field: CREATED_AT, direction: ASC} ) {
      edges {
        node {
          headRefName
          number
          title
          author {
            login
          }
        }
      }
    }
  }
}
)";
}

void FGitHubToolsHttpRequest_GetOpenedPullRequests::ParseResponseData( const FJsonObject & json_data )
{
    const auto repository_object = json_data.GetObjectField( TEXT( "repository" ) );
    const auto pull_requests_objects = repository_object->GetObjectField( TEXT( "pullRequests" ) );
    const auto pull_requests_edges_objects = pull_requests_objects->GetArrayField( TEXT( "edges" ) );

    TArray< FGitHubToolsOpenedPullRequestInfosPtr > opened_prs;
    opened_prs.Reserve( pull_requests_edges_objects.Num() );

    for ( const auto pull_request_infos : pull_requests_edges_objects )
    {
        const auto pr_object = pull_request_infos->AsObject();
        const auto node_object = pr_object->GetObjectField( TEXT( "node" ) );

        opened_prs.Emplace( MakeShared< FGitHubToolsOpenedPullRequestInfos >( node_object.ToSharedRef() ) );
    }

    Result = opened_prs;
}

#undef LOCTEXT_NAMESPACE