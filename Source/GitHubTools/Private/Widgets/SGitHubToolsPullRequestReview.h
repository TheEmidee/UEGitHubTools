// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GitHubToolsReviewFileItem.h"
#include "GitHubToolsTypes.h"
#include "GitSourceControlState.h"

#include <CoreMinimal.h>

class SGitHubToolsPullRequestReview final : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS( SGitHubToolsPullRequestReview ) :
        _ParentWindow()
    //_Items()
    {}

    SLATE_ATTRIBUTE( TSharedPtr< SWindow >, ParentWindow )
    SLATE_ATTRIBUTE( TArray< FGithubToolsPullRequestFileInfosPtr >, Files )

    SLATE_END_ARGS()

    virtual ~SGitHubToolsPullRequestReview();

    void Construct( const FArguments & arguments );

    virtual FReply OnKeyDown( const FGeometry & my_geometry, const FKeyEvent & key_event ) override;
    TSharedRef< SWidget > GenerateWidgetForItemAndColumn( FGithubToolsPullRequestFileInfosPtr item, const FName column_id );

private:
    bool IsFileCommentsButtonEnabled() const;
    EVisibility IsWarningPanelVisible() const;

    FReply CancelClicked();
    FReply OnFileCommentsButtonClicked( FGithubToolsPullRequestFileInfosPtr item );

    TSharedRef< ITableRow > OnGenerateRowForList( FGithubToolsPullRequestFileInfosPtr SubmitItemData, const TSharedRef< STableViewBase > & owner_table );
    EColumnSortMode::Type GetColumnSortMode( const FName column_id ) const;
    void OnColumnSortModeChanged( const EColumnSortPriority::Type sort_priority, const FName & column_id, const EColumnSortMode::Type sort_mode );
    void RequestSort();
    void SortTree();
    void OnDiffAgainstRemoteStatusBranchSelected( FGithubToolsPullRequestFileInfosPtr selected_item );

    TSharedPtr< SListView< FGithubToolsPullRequestFileInfosPtr > > ListView;
    TArray< FGithubToolsPullRequestFileInfosPtr > ListViewItems;
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