#pragma once

#include "GitHubToolsTypes.h"

#include <CoreMinimal.h>
#include <Widgets/Views/STableRow.h>

DECLARE_DELEGATE_OneParam( FGitHubToolsOnPendingReviewUpdated, FGithubToolsPullRequestPendingReviewInfosPtr )

class SGitHubToolsPRPendingReviewTableRow : public STableRow< FGithubToolsPullRequestPendingReviewInfosPtr >
{
public:
    SLATE_BEGIN_ARGS( SGitHubToolsPRPendingReviewTableRow )
    {}

    SLATE_ARGUMENT( FGithubToolsPullRequestInfosPtr, PRInfos )
    SLATE_ARGUMENT( FGithubToolsPullRequestPendingReviewInfosPtr, PendingReview )
    SLATE_EVENT( FGitHubToolsOnPendingReviewUpdated, OnReviewStateUpdated )

    SLATE_END_ARGS()

    void Construct( const FArguments & arguments, const TSharedRef< STableViewBase > & owner_table_view );

private:
    TSharedRef< ITableRow > GenerateCommentRow( FGithubToolsPullRequestCommentPtr item, const TSharedRef< STableViewBase > & owner_table );
    FReply OnApproveReviewButtonClicked();
    FReply OnAbandonReviewButtonClicked();

    FGithubToolsPullRequestInfosPtr PRInfos;
    FGithubToolsPullRequestPendingReviewInfosPtr PendingReview;
    FGitHubToolsOnPendingReviewUpdated OnReviewStateUpdated;
};