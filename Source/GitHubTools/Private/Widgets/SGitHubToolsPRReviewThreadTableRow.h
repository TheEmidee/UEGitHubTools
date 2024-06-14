#pragma once

#include "GitHubToolsTypes.h"

#include <CoreMinimal.h>
#include <Widgets/Views/STableRow.h>

class SGitHubToolsPRReviewThreadTableRow : public STableRow< FGithubToolsPullRequestReviewThreadInfosPtr >
{
public:
    SLATE_BEGIN_ARGS( SGitHubToolsPRReviewThreadTableRow )
    {}

    SLATE_ARGUMENT( FGithubToolsPullRequestReviewThreadInfosPtr, ThreadInfos )
    SLATE_EVENT( FOnClicked, OnAddCommentClicked )

    SLATE_END_ARGS()

    void Construct( const FArguments & arguments, const TSharedRef< STableViewBase > & owner_table_view );

private:
    FSlateColor GetBorderBackgroundColor() const;
    TSharedRef< ITableRow > GenerateCommentRow( FGithubToolsPullRequestCommentPtr item, const TSharedRef< STableViewBase > & owner_table );
    FReply OnResolveConversationClicked();
    FText GetCollapsedButtonText() const;
    FReply OnCollapsedButtonClicked();

    FGithubToolsPullRequestReviewThreadInfosPtr ThreadInfos;
    TSharedPtr< SListView< FGithubToolsPullRequestCommentPtr > > CommentsListView;
    TSharedPtr< SVerticalBox > CommentsPanel;
    FOnClicked OnAddCommentButtonClicked;
};