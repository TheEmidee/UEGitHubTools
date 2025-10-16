#pragma once

#include "CoreMinimal.h"
#include "GitHubToolsTypes.h"
#include "SGitHubToolsAddCommentForm.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"
#include "Widgets/Views/SListView.h"

class SWidgetSwitcher;
class SCheckBox;
class SVerticalBox;
class SGitHubToolsPRReviewList final : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS( SGitHubToolsPRReviewList )
    {}

    SLATE_ATTRIBUTE( TSharedPtr< SWindow >, ParentWindow )
    SLATE_ATTRIBUTE( FGithubToolsPullRequestInfosPtr, PRInfos )
    SLATE_EVENT( FSimpleDelegate, OnShouldRebuildFileTreeView )

    SLATE_END_ARGS()

    virtual ~SGitHubToolsPRReviewList() override;

    void Construct( const FArguments & arguments );
    void ShowFileReviews( const FGithubToolsPullRequestFileInfosPtr & file_infos );

private:
    FReply OnAddCommentClicked( FGithubToolsPullRequestReviewThreadInfosPtr thread_infos );
    TSharedRef< ITableRow > GenerateItemRow( FGithubToolsPullRequestReviewThreadInfosPtr item, const TSharedRef< STableViewBase > & owner_table );
    void OnHideResolvedThreadsCheckStateChanged( ECheckBoxState new_state );
    bool CanSubmitComment() const;
    FReply OnCreateNewThreadButtonClicked();
    void ShowAddCommentWindow( const FGithubToolsPullRequestReviewThreadInfosPtr & thread_infos );

    TWeakPtr< SWindow > ParentFrame;
    FGithubToolsPullRequestInfosPtr PRInfos;
    FGithubToolsPullRequestFileInfosPtr FileInfos;
    TSharedPtr< SMultiLineEditableTextBox > ChangeListDescriptionTextCtrl;
    TSharedPtr< SVerticalBox > AllCommentsVerticalBox;
    TSharedPtr< SListView< TSharedPtr< FGithubToolsPullRequestReviewThreadInfos > > > ReviewThreadsListView;
    TArray< TSharedPtr< FGithubToolsPullRequestReviewThreadInfos > > ReviewThreads;
    TSharedPtr< SCheckBox > HideResolvedThreadsCheckBox;
    TSharedPtr< SGitHubToolsAddCommentForm > AddCommentForm;
    TSharedPtr< SWidgetSwitcher > WidgetSwitcher;
    FSimpleDelegate OnShouldRebuildFileTreeView;
};