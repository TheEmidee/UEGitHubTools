#include "GitHubToolsHttpRequest_GetPullRequestInfos.h"

#include "Dom/JsonValue.h"
#include "GitHubToolsGitUtils.h"
#include "Serialization/JsonSerializer.h"

#define LOCTEXT_NAMESPACE "GitHubTools.Requests"

FGitHubToolsHttpRequestData_GetPullRequestInfos::FGitHubToolsHttpRequestData_GetPullRequestInfos( int pull_request_number, TArray< FGithubToolsPullRequestFileInfosPtr > files, TArray< FGithubToolsPullRequestFilePatchPtr > patches ) :
    PullRequestNumber( pull_request_number ),
    Files( MoveTemp( files ) ),
    Patches( MoveTemp( patches ) )
{
}

FString FGitHubToolsHttpRequestData_GetPullRequestInfos::GetRawQuery() const
{
    return R"(
query ($repoOwner: String!, $repoName: String!, $pullNumber: Int!) {
  viewer {
    login
  }
  repository( owner: $repoOwner, name: $repoName) {
    pullRequest( number : $pullNumber ) {
      number
      id
      title
      author {
        login
      }
      baseRefName
      bodyText 
      changedFiles 
      createdAt 
      body 
      headRefName 
      isDraft 
      mergeable 
      state 
      url 
      commits {
        totalCount
      }
      reviewThreads( first : 100 ) {
        nodes {
          resolvedBy {
            login
          } 
          id
          isResolved
          path
          subjectType
          diffSide
          line
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
      checks: commits( last: 1 ) {
        edges {
          node {
            commit {
              status {
                contexts {
                  context
                  state
                  description
                }
              }
            }
          }
        }
      }
      reviews( first : 100 ) {
        edges {
          node {
            author {
              login
            }
            id
            state
            comments( first : 100 ) {
              edges {
                node {
                  author {
                    login
                  }
                  id
                  body
                  createdAt
                  path
                }
              }
            }
          }
        }
      }
    }
  }
}
)";
}

void FGitHubToolsHttpRequestData_GetPullRequestInfos::ParseResponseData( const FJsonObject & json_data )
{
    const auto viewer_object = json_data.GetObjectField( TEXT( "viewer" ) );
    const auto repository_object = json_data.GetObjectField( TEXT( "repository" ) );

    const auto pull_request_object = repository_object->GetObjectField( TEXT( "pullRequest" ) );

    auto pr_infos = MakeShared< FGithubToolsPullRequestInfos >( pull_request_object.ToSharedRef() );
    pr_infos->ViewerLogin = viewer_object->GetStringField( TEXT( "login" ) );

    const auto review_threads_object = pull_request_object->GetObjectField( TEXT( "reviewThreads" ) );
    const auto review_threads_nodes_object = review_threads_object->GetArrayField( TEXT( "nodes" ) );

    TArray< FGithubToolsPullRequestReviewThreadInfosPtr > reviews;
    reviews.Reserve( review_threads_nodes_object.Num() );

    FGithubToolsPullRequestReviewThreadInfosPtr user_review;

    for ( const auto review_thread_object : review_threads_nodes_object )
    {
        const auto review_thread_node_object = review_thread_object->AsObject();

        auto review_thread_infos = MakeShared< FGithubToolsPullRequestReviewThreadInfos >( review_thread_node_object.ToSharedRef() );
        review_thread_infos->PRNumber = pr_infos->Number;

        const auto comments_object = review_thread_node_object->GetObjectField( TEXT( "comments" ) );
        const auto comments_edges_object = comments_object->GetArrayField( TEXT( "edges" ) );

        review_thread_infos->Comments.Reserve( comments_edges_object.Num() );

        for ( const auto comment_object : comments_edges_object )
        {
            const auto comment_node_object = comment_object->AsObject()->GetObjectField( TEXT( "node" ) );

            auto comment = MakeShared< FGithubToolsPullRequestComment >( comment_node_object.ToSharedRef() );

            review_thread_infos->Comments.Emplace( comment );
        }

        if ( !review_thread_infos->bIsResolved )
        {
            pr_infos->bHasUnresolvedConversations = true;
        }

        reviews.Emplace( review_thread_infos );
    }

    pr_infos->SetFiles( Files, Patches, reviews );

    const auto checks_object = pull_request_object->GetObjectField( TEXT( "checks" ) );
    const auto checks_edges_object = checks_object->GetArrayField( TEXT( "edges" ) );
    const auto check_node_object = checks_edges_object[ 0 ]->AsObject()->GetObjectField( TEXT( "node" ) );
    const auto commit_object = check_node_object->GetObjectField( TEXT( "commit" ) );
    const auto status_json = commit_object->GetObjectField( TEXT( "status" ) );
    const auto contexts_json = status_json->GetArrayField( TEXT( "contexts" ) );

    pr_infos->Checks.Reserve( contexts_json.Num() );

    for ( const auto check_object : contexts_json )
    {
        pr_infos->Checks.Emplace( MakeShared< FGitHubToolsPullRequestCheckInfos >( check_object->AsObject().ToSharedRef() ) );
    }

    const auto reviews_object = pull_request_object->GetObjectField( TEXT( "reviews" ) );
    const auto reviews_edges_object = reviews_object->GetArrayField( TEXT( "edges" ) );

    for ( const auto review_edge_object : reviews_edges_object )
    {
        const auto review_node_object = review_edge_object->AsObject()->GetObjectField( TEXT( "node" ) );
        const auto review_node_author_object = review_node_object->GetObjectField( TEXT( "author" ) );
        const auto review_node_author_login = review_node_author_object->GetStringField( TEXT( "login" ) );

        if ( review_node_author_login != pr_infos->ViewerLogin )
        {
            continue;
        }

        const auto state = GitHubToolsUtils::GetPullRequestReviewState( review_node_object->GetStringField( TEXT( "state" ) ) );
        if ( state == EGitHubToolsPullRequestReviewState::Approved )
        {
            pr_infos->bApprovedByMe = true;
            continue;
        }

        if ( state != EGitHubToolsPullRequestReviewState::Pending )
        {
            continue;
        }

        auto pending_review = MakeShared< FGithubToolsPullRequestPendingReviewInfos >();
        pending_review->Id = review_node_object->GetStringField( TEXT( "id" ) );

        const auto comments_object = review_node_object->GetObjectField( TEXT( "comments" ) );
        const auto comments_edges_object = comments_object->GetArrayField( TEXT( "edges" ) );

        if ( !ensureAlwaysMsgf( pr_infos->PendingReview == nullptr, TEXT( "There can only be one review per user per pull request !" ) ) )
        {
            continue;
        }

        pr_infos->PendingReview = pending_review;

        pending_review->Comments.Reserve( comments_edges_object.Num() );

        for ( const auto comment_object : comments_edges_object )
        {
            const auto comment_node_object = comment_object->AsObject()->GetObjectField( TEXT( "node" ) );

            auto comment = MakeShared< FGithubToolsPullRequestComment >( comment_node_object.ToSharedRef() );

            pending_review->Comments.Emplace( comment );
        }
    }

    Result = pr_infos;
}

void FGitHubToolsHttpRequestData_GetPullRequestInfos::AddParameters( FJsonObject & variables_object ) const
{
    FGitHubToolsHttpRequestGraphQLQuery::AddParameters( variables_object );
    variables_object.SetNumberField( TEXT( "pullNumber" ), PullRequestNumber );
}

#undef LOCTEXT_NAMESPACE