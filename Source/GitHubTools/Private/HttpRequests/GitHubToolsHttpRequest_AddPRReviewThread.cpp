#include "GitHubToolsHttpRequest_AddPRReviewThread.h"

#include "Dom/JsonValue.h"
#include "GitHubToolsTypes.h"
#include "Serialization/JsonSerializer.h"

#define LOCTEXT_NAMESPACE "GitHubTools.Requests"

FGitHubToolsHttpRequestData_AddPRReviewThread::FGitHubToolsHttpRequestData_AddPRReviewThread( const FString & pull_request_id, const FString & pull_request_review_id, const FString & file_path, const FString & comment ) :
    PullRequestId( pull_request_id ),
    PullRequestReviewId( pull_request_review_id ),
    FilePath( file_path ),
    Comment( comment )
{
}

FString FGitHubToolsHttpRequestData_AddPRReviewThread::GetRawQuery() const
{
    return FString::Printf(TEXT(R"(
mutation AddPullRequestReviewThread( 
  $body: String!, 
  $pullRequestId: ID!, 
  $pullRequestReviewId : ID!,
  $path: String!,
  $subjectType: PullRequestReviewThreadSubjectType!
  %s
  ) {
  addPullRequestReviewThread( 
    input: { 
      body: $body,
      pullRequestId: $pullRequestId, 
      pullRequestReviewId: $pullRequestReviewId, 
      path: $path,
      subjectType: $subjectType
      %s
      } 
  ) { 
    thread {
      resolvedBy {
        login
      } 
      id
      isResolved
      path
      diffSide
      subjectType
      line
      pullRequest {
         number
      }
      comments( first : 100 ) {
        edges {
          node {
            author {
              login
            } 
            id
            body
            createdAt
          }
        }
      }
    }
  }
}
)"), *GetMutationAdditionalParameters(), *GetInputAdditionalParameters() );
}

void FGitHubToolsHttpRequestData_AddPRReviewThread::ParseResponseData( const FJsonObject & json_data )
{
    const auto result_object = json_data.GetObjectField( TEXT( "addPullRequestReviewThread" ) );
    const auto thread_object = result_object->GetObjectField( TEXT( "thread" ) );
    const auto pr_object = thread_object->GetObjectField( TEXT( "pullRequest" ) );

    auto review_thread_infos = MakeShared< FGithubToolsPullRequestReviewThreadInfos >( thread_object.ToSharedRef() );
    review_thread_infos->PRNumber = pr_object->GetIntegerField( TEXT( "number" ) );

    const auto comments_object = thread_object->GetObjectField( TEXT( "comments" ) );
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

    Result = review_thread_infos;
}

void FGitHubToolsHttpRequestData_AddPRReviewThread::AddParameters( FJsonObject & variables_object ) const
{
    FGitHubToolsHttpRequestGraphQLMutation::AddParameters( variables_object );
    variables_object.SetStringField( TEXT( "pullRequestId" ), PullRequestId );
    variables_object.SetStringField( TEXT( "pullRequestReviewId" ), PullRequestReviewId );
    variables_object.SetStringField( TEXT( "body" ), Comment );
    variables_object.SetStringField( TEXT( "path" ), FilePath );
}

FGitHubToolsHttpRequestData_AddPRReviewThreadToFile::FGitHubToolsHttpRequestData_AddPRReviewThreadToFile( const FString & pull_request_id, const FString & pull_request_review_id, const FString & file_path, const FString & comment ) :
    FGitHubToolsHttpRequestData_AddPRReviewThread( pull_request_id, pull_request_review_id, file_path, comment )
{
}

void FGitHubToolsHttpRequestData_AddPRReviewThreadToFile::AddParameters( FJsonObject & variables_object ) const
{
    FGitHubToolsHttpRequestData_AddPRReviewThread::AddParameters( variables_object );
    variables_object.SetStringField( TEXT( "subjectType" ), TEXT( "FILE" ) );
}

FGitHubToolsHttpRequestData_AddPRReviewThreadToLine::FGitHubToolsHttpRequestData_AddPRReviewThreadToLine( const FString & pull_request_id, const FString & pull_request_review_id, const FString & file_path, const EGitHubToolsDiffSide diff_side, const int line, const FString & comment ) :
    FGitHubToolsHttpRequestData_AddPRReviewThread( pull_request_id, pull_request_review_id, file_path, comment ),
    DiffSide( diff_side ),
    Line( line )
{
}

void FGitHubToolsHttpRequestData_AddPRReviewThreadToLine::AddParameters( FJsonObject & variables_object ) const
{
    const auto get_side_str = [ & ]() -> FString {
        return DiffSide == EGitHubToolsDiffSide::Left
                   ? TEXT( "LEFT" )
                   : TEXT( "RIGHT" );
    };

    FGitHubToolsHttpRequestData_AddPRReviewThread::AddParameters( variables_object );
    variables_object.SetStringField( TEXT( "subjectType" ), TEXT( "LINE" ) );
    variables_object.SetStringField( TEXT( "side" ), get_side_str() );
    variables_object.SetNumberField( TEXT( "line" ), Line );
}

FString FGitHubToolsHttpRequestData_AddPRReviewThreadToLine::GetMutationAdditionalParameters() const
{
    return R"(
  ,
  $line: Int!,
  $side: DiffSide!
)";
}

FString FGitHubToolsHttpRequestData_AddPRReviewThreadToLine::GetInputAdditionalParameters() const
{
    return R"(
  ,
  line: $line,
  side: $side
)";
}

#undef LOCTEXT_NAMESPACE