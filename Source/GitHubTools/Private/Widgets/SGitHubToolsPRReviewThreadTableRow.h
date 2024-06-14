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

    SLATE_END_ARGS()

    void Construct( const FArguments & arguments, const TSharedRef< STableViewBase > & owner_table_view );

private:
    FSlateColor GetBorderBackgroundColor() const;
    TSharedRef< ITableRow > GenerateCommentRow( FGithubToolsPullRequestCommentPtr item, const TSharedRef< STableViewBase > & owner_table );
    FReply OnResolveConversationClicked();

    FGithubToolsPullRequestReviewThreadInfosPtr ThreadInfos;
    TSharedPtr< SListView< FGithubToolsPullRequestCommentPtr > > CommentsListView;
};