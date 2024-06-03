#include "GitHubToolsHttpRequest.h"

#include "GitSourceControlModule.h"
#include "Interfaces/IHttpResponse.h"

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

// TFuture< int > FGithubToolsRequest_GetPullRequestNumber::GetPullRequestNumber()
//{
//     SendRequest(
//         EGitHubToolsRequestType::GET,
//         TEXT( "pulls" ),
//         FHttpRequestCompleteDelegate::CreateLambda( [ & ]( FHttpRequestPtr request, FHttpResponsePtr response, bool success ) {
//             const auto local_branch_name = FGitSourceControlModule::Get().GetProvider().GetBranchName();
//
//             const auto json_response = response->GetContentAsString();
//
//             const auto json_reader = TJsonReaderFactory<>::Create( json_response );
//
//             TArray< TSharedPtr< FJsonValue > > pull_requests_infos;
//             if ( !FJsonSerializer::Deserialize( json_reader, pull_requests_infos ) )
//             {
//                 Promise.SetValue( INDEX_NONE );
//             }
//
//             for ( const auto pull_request_infos : pull_requests_infos )
//             {
//                 const auto pr_object = pull_request_infos->AsObject();
//                 const int number = pr_object->GetIntegerField( TEXT( "number" ) );
//                 const auto head_object = pr_object->GetObjectField( TEXT( "head" ) );
//                 const auto ref = head_object->GetStringField( TEXT( "ref" ) );
//
//                 if ( ref == local_branch_name )
//                 {
//                     Promise.SetValue( number );
//                     return;
//                 }
//             }
//
//             Promise.SetValue( INDEX_NONE );
//         } ) );
//
//     return Promise.GetFuture();
// }
//
// TFuture< TArray< FString > > FGithubToolsRequest_GetPullRequestFiles::GetPullRequestFiles( int pull_request_number )
//{
//     SendRequest(
//         EGitHubToolsRequestType::GET,
//         FString::Format( TEXT( "pulls/%i/files" ), { pull_request_number } ),
//         FHttpRequestCompleteDelegate::CreateLambda( [ & ]( FHttpRequestPtr request, FHttpResponsePtr response, bool success ) {
//             const auto local_branch_name = FGitSourceControlModule::Get().GetProvider().GetBranchName();
//
//             const auto json_response = response->GetContentAsString();
//
//             const auto json_reader = TJsonReaderFactory<>::Create( json_response );
//
//             TArray< TSharedPtr< FJsonValue > > files_infos;
//             if ( !FJsonSerializer::Deserialize( json_reader, files_infos ) )
//             {
//                 Promise.SetValue( {} );
//             }
//
//             TArray< FString > filenames;
//             filenames.Reserve( files_infos.Num() );
//
//             for ( const auto file_infos : files_infos )
//             {
//                 const auto file_infos_object = file_infos->AsObject();
//                 filenames.Emplace( file_infos_object->GetStringField( TEXT( "filename" ) ) );
//             }
//
//             Promise.SetValue( filenames );
//         } ) );
//
//     return Promise.GetFuture();
// }
