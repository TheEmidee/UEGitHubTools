#pragma once

#include "GitHubToolsTypes.h"
#include "SGitHubToolsAddCommentForm.h"
#include "Interfaces/IHttpRequest.h"

#include <CoreMinimal.h>
#include <Widgets/Input/SMultiLineEditableTextBox.h>

class SGitHubToolsPRReviewList : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS( SGitHubToolsPRReviewList ) :
        _ParentWindow(),
        _PRInfos()
    {}

    SLATE_ATTRIBUTE( TSharedPtr< SWindow >, ParentWindow )
    SLATE_ATTRIBUTE( FGithubToolsPullRequestInfosPtr, PRInfos )

    SLATE_END_ARGS()

    virtual ~SGitHubToolsPRReviewList() override;

    void Construct( const FArguments & arguments );
    void ShowFileReviews( FGithubToolsPullRequestFileInfosPtr file_infos );

private:
    FReply OnAddCommentClicked( FGithubToolsPullRequestReviewThreadInfosPtr thread_infos );
    TSharedRef< ITableRow > GenerateItemRow( FGithubToolsPullRequestReviewThreadInfosPtr item, const TSharedRef< STableViewBase > & owner_table );
    void OnHideResolvedThreadsCheckStateChanged( ECheckBoxState new_state );
    bool CanSubmitComment() const;
    FReply OnSubmitCommentCliked();
    FReply OnCancelCommentClicked();
    FReply OnCreateNewThreadButtonClicked();

    TWeakPtr< SWindow > ParentFrame;
    FGithubToolsPullRequestInfosPtr PRInfos;
    TSharedPtr< SMultiLineEditableTextBox > ChangeListDescriptionTextCtrl;
    TSharedPtr< SVerticalBox > AllCommentsVerticalBox;
    TSharedPtr< SListView< TSharedPtr< FGithubToolsPullRequestReviewThreadInfos > > > ReviewThreadsListView;
    TArray< TSharedPtr< FGithubToolsPullRequestReviewThreadInfos > > ReviewThreads;
    TSharedPtr< SCheckBox > HideResolvedThreadsCheckBox;
    TSharedPtr< SWidgetSwitcher > WidgetSwitcher;
    TSharedPtr< SGitHubToolsAddCommentForm > AddCommentForm;
};