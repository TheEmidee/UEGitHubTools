#pragma once

#include "GithubToolsTypes.h"
#include "SGitHubToolsAddCommentForm.h"

#include <CoreMinimal.h>

class SGitHubToolsAddCommentForm;

struct FGitHubToolsFilePatchViewListItem : public TSharedFromThis< FGitHubToolsFilePatchViewListItem >
{
public:
    FGitHubToolsFilePatchViewListItem() = default;
    explicit FGitHubToolsFilePatchViewListItem( FGithubToolsPullRequestFileInfosPtr file_infos, FString && string, TOptional< EGitHubToolsDiffSide > diff_side, int left_side_line_number, int right_side_line_number );

    TSharedRef< SWidget > GenerateWidgetForItem();

private:
    FReply OnAddCommentClicked();
    TSharedRef< ITableRow > GenerateItemRow( FGithubToolsPullRequestReviewThreadInfosPtr thread_infos, const TSharedRef< STableViewBase > & owner_table );
    EVisibility GetThreadListVisibility() const;
    void SetReviews();
    FGitHubToolsAddCommentLineInfos GetCommentLineInfos() const;

    FGithubToolsPullRequestFileInfosPtr FileInfos;
    TSharedPtr< SGitHubToolsAddCommentForm > AddCommentForm;
    TOptional< EGitHubToolsDiffSide > DiffSide;
    FString String;
    int LeftLine;
    int RightLine;
    TArray< TSharedPtr< FGithubToolsPullRequestReviewThreadInfos > > Reviews;
    TSharedPtr< SListView< TSharedPtr< FGithubToolsPullRequestReviewThreadInfos > > > ThreadList;
};

typedef TSharedPtr< FGitHubToolsFilePatchViewListItem > FGitHubToolsFilePatchViewListItemPtr;

class SGitHubToolsFilePatch final : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS( SGitHubToolsFilePatch )
    {}

    SLATE_ATTRIBUTE( FGithubToolsPullRequestFileInfosPtr, FileInfos )
    SLATE_ARGUMENT( TSharedPtr< SWindow >, ParentWindow )

    SLATE_END_ARGS()

    virtual ~SGitHubToolsFilePatch() override;

    void Construct( const FArguments & arguments );

private:
    TSharedRef< ITableRow > GenerateRowForItem( FGitHubToolsFilePatchViewListItemPtr item, const TSharedRef< STableViewBase > & owner_table ) const;
    void PopulateListItems();
    void OnMouseButtonClick( TSharedPtr< FGitHubToolsFilePatchViewListItem > item );

    FGithubToolsPullRequestFileInfosPtr FileInfos;
    TSharedPtr< SListView< FGitHubToolsFilePatchViewListItemPtr > > ListView;
    TArray< FGitHubToolsFilePatchViewListItemPtr > ListItems;

    TWeakPtr< SWindow > WeakParentWindow;
};

#undef LOCTEXT_NAMESPACE