#pragma once

#include "GitHubToolsTypes.h"

#include <CoreMinimal.h>

class SGitSourceControlReviewFilesListRow;

class SGitHubToolsPullRequestReview final : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS( SGitHubToolsPullRequestReview ) :
        _ParentWindow()
    //_Items()
    {}

    SLATE_ATTRIBUTE( TSharedPtr< SWindow >, ParentWindow )
    SLATE_ATTRIBUTE( FGithubToolsPullRequestInfosPtr, Infos )

    SLATE_END_ARGS()

    virtual ~SGitHubToolsPullRequestReview() override;

    void Construct( const FArguments & arguments );
    FReply OnKeyDown( const FGeometry & my_geometry, const FKeyEvent & key_event ) override;

private:
    bool IsFileCommentsButtonEnabled() const;
    EVisibility IsWarningPanelVisible() const;
    FReply CancelClicked();
    FReply OnFileCommentsButtonClicked( FGithubToolsPullRequestFileInfosPtr item );
    TSharedRef< ITableRow > OnGenerateRowForList( FGithubToolsPullRequestFileInfosPtr SubmitItemData, const TSharedRef< STableViewBase > & owner_table );
    EVisibility GetItemRowVisibility( FGithubToolsPullRequestFileInfosPtr file_infos ) const;
    EColumnSortMode::Type GetColumnSortMode( const FName column_id ) const;
    void OnColumnSortModeChanged( const EColumnSortPriority::Type sort_priority, const FName & column_id, const EColumnSortMode::Type sort_mode );
    void RequestSort();
    void SortTree();
    void OnDiffAgainstRemoteStatusBranchSelected( FGithubToolsPullRequestFileInfosPtr selected_item );
    FReply OpenInGitHubClicked();
    void OnShowOnlyUAssetsCheckStateChanged( ECheckBoxState new_state );
    void OnHideOFPACheckStateChanged( ECheckBoxState new_state );

    FGithubToolsPullRequestInfosPtr PRInfos;
    TSharedPtr< SListView< FGithubToolsPullRequestFileInfosPtr > > ListView;
    TSharedPtr< SCheckBox > OnlyShowAssetsCheckBox;
    TSharedPtr< SCheckBox > HideOFPACheckBox;
    TWeakPtr< SWindow > ParentFrame;
    FName SortByColumn;
    EColumnSortMode::Type SortMode = EColumnSortMode::Ascending;
};

class SGitSourceControlReviewFilesListRow : public SMultiColumnTableRow< FGithubToolsPullRequestFileInfosPtr >
{
public:
    SLATE_BEGIN_ARGS( SGitSourceControlReviewFilesListRow )
    {}

    SLATE_ARGUMENT( TSharedPtr< SGitHubToolsPullRequestReview >, SourceControlSubmitWidget )
    SLATE_ARGUMENT( FGithubToolsPullRequestFileInfosPtr, FileInfos )

    SLATE_END_ARGS()

    void Construct( const FArguments & arguments, const TSharedRef< STableViewBase > & owner_table_view );
    virtual TSharedRef< SWidget > GenerateWidgetForColumn( const FName & column_name ) override;

private:
    TWeakPtr< SGitHubToolsPullRequestReview > SourceControlSubmitWidgetPtr;
    FGithubToolsPullRequestFileInfosPtr FileInfos;
};

#undef LOCTEXT_NAMESPACE