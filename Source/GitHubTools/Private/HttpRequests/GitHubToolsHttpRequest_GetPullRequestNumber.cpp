#include "GitHubToolsHttpRequest_GetPullRequestNumber.h"

#include "GitHubToolsSettings.h"
#include "GitSourceControlModule.h"

#include <Interfaces/IHttpResponse.h>

#define LOCTEXT_NAMESPACE "GitHubTools.Requests"

FString FGitHubToolsHttpRequestData_GetPullRequestNumber::GetBody() const
{
    const auto * settings = GetDefault< UGitHubToolsSettings >();

    TStringBuilder< 512 > string_builder;

    string_builder << TEXT( "{ \"query\" : \"query ( $repoOwner: String!, $repoName: String! ) {" );
    string_builder << TEXT( "  repository(owner: $repoOwner, name: $repoName) {" );
    string_builder << TEXT( "    pullRequests( last: 100 ) {" );
    string_builder << TEXT( "      edges {" );
    string_builder << TEXT( "        node {" );
    string_builder << TEXT( "          headRefName" );
    string_builder << TEXT( "          number" );
    string_builder << TEXT( "        }" );
    string_builder << TEXT( "      }" );
    string_builder << TEXT( "    }" );
    string_builder << TEXT( "  }" );
    string_builder << TEXT( "}" );
    string_builder << TEXT( "\"," );
    string_builder << TEXT( "\"variables\": " );
    string_builder << TEXT( "  {" );
    string_builder << TEXT( "    \"repoOwner\": \"" << settings->RepositoryOwner << "\"," );
    string_builder << TEXT( "    \"repoName\": \"" << settings->RepositoryName << "\"," );
    string_builder << TEXT( "  }" );
    string_builder << TEXT( "}" );

    return *string_builder;
}

void FGitHubToolsHttpRequestData_GetPullRequestNumber::ParseResponse( FHttpResponsePtr response_ptr )
{
    const auto local_branch_name = FGitSourceControlModule::Get().GetProvider().GetBranchName();
    const auto json_response = response_ptr->GetContentAsString();
    const auto json_reader = TJsonReaderFactory<>::Create( json_response );

    TSharedPtr< FJsonValue > data_node;
    if ( !FJsonSerializer::Deserialize( json_reader, data_node ) )
    {
        return;
    }

    const auto data_node_object = data_node->AsObject()->GetObjectField( TEXT( "data" ) );
    const auto repository_object = data_node_object->GetObjectField( TEXT( "repository" ) );
    const auto pull_requests_objects = repository_object->GetObjectField( TEXT( "pullRequests" ) );
    const auto pull_requests_edges_objects = pull_requests_objects->GetArrayField( TEXT( "edges" ) );

    for ( const auto pull_request_infos : pull_requests_edges_objects )
    {
        const auto pr_object = pull_request_infos->AsObject();
        const auto node_object = pr_object->GetObjectField( TEXT( "node" ) );

        const int number = node_object->GetIntegerField( TEXT( "number" ) );
        const auto ref = node_object->GetStringField( TEXT( "headRefName" ) );

        if ( ref == local_branch_name )
        {
            Result = number;
            return;
        }
    }

    Result = INDEX_NONE;
}

#undef LOCTEXT_NAMESPACE