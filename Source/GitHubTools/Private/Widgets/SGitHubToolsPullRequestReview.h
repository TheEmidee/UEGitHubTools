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

    /** Constructs the widget */
    void Construct( const FArguments & arguments );

    /** Used to intercept Escape key press, and interpret it as cancel */
    virtual FReply OnKeyDown( const FGeometry & my_geometry, const FKeyEvent & key_event ) override;

    /** Get dialog result */
    // ESubmitResults::Type GetResult() { return DialogResult; }

    /** Returns a widget representing the item and column supplied */
    TSharedRef< SWidget > GenerateWidgetForItemAndColumn( FGithubToolsPullRequestFileInfosPtr item, const FName column_id );

private:
    bool IsFileCommentsButtonEnabled() const;
    EVisibility IsWarningPanelVisible() const;

    //	/** Called when the settings of the dialog are to be ignored*/
    FReply CancelClicked();
    FReply OnFileCommentsButtonClicked( FGithubToolsPullRequestFileInfosPtr item );

    /** Called by SListView to get a widget corresponding to the supplied item */
    TSharedRef< ITableRow > OnGenerateRowForList( FGithubToolsPullRequestFileInfosPtr SubmitItemData, const TSharedRef< STableViewBase > & owner_table );

    /**
     * Returns the current column sort mode (ascending or descending) if the ColumnId parameter matches the current
     * column to be sorted by, otherwise returns EColumnSortMode_None.
     *
     * @param	column_id	Column ID to query sort mode for.
     *
     * @return	The sort mode for the column, or EColumnSortMode_None if it is not known.
     */
    EColumnSortMode::Type GetColumnSortMode( const FName column_id ) const;

    /**
     * Callback for SHeaderRow::Column::OnSort, called when the column to sort by is changed.
     *
     * @param	column_id	The new column to sort by
     * @param	sort_mode	The sort mode (ascending or descending)
     */
    void OnColumnSortModeChanged( const EColumnSortPriority::Type sort_priority, const FName & column_id, const EColumnSortMode::Type sort_mode );

    /**
     * Requests that the source list data be sorted according to the current sort column and mode,
     * and refreshes the list view.
     */
    void RequestSort();

    /**
     * Sorts the source list data according to the current sort column and mode.
     */
    void SortTree();

    void OnDiffAgainstRemoteStatusBranchSelected( FGithubToolsPullRequestFileInfosPtr selected_item );

    /** ListBox for selecting which object to consolidate */
    TSharedPtr< SListView< FGithubToolsPullRequestFileInfosPtr > > ListView;

    /** Collection of objects (Widgets) to display in the List View. */
    TArray< FGithubToolsPullRequestFileInfosPtr > ListViewItems;

    /** Pointer to the parent modal window */
    TWeakPtr< SWindow > ParentFrame;

    /** Specify which column to sort with */
    FName SortByColumn;

    /** Currently selected sorting mode */
    EColumnSortMode::Type SortMode = EColumnSortMode::Ascending;
};

class SGitSourceControlReviewFilesListRow : public SMultiColumnTableRow< FGithubToolsPullRequestFileInfosPtr >
{
public:
    SLATE_BEGIN_ARGS( SGitSourceControlReviewFilesListRow )
    {}

    /** The SSourceControlSubmitWidget that owns the tree.  We'll only keep a weak reference to it. */
    SLATE_ARGUMENT( TSharedPtr< SGitHubToolsPullRequestReview >, SourceControlSubmitWidget )

    /** The list item for this row */
    SLATE_ARGUMENT( FGithubToolsPullRequestFileInfosPtr, FileInfos )

    SLATE_END_ARGS()

    /** Construct function for this widget */
    void Construct( const FArguments & arguments, const TSharedRef< STableViewBase > & owner_table_view );

    /** Overridden from SMultiColumnTableRow.  Generates a widget for this column of the list row. */
    virtual TSharedRef< SWidget > GenerateWidgetForColumn( const FName & column_name ) override;

private:
    /** Weak reference to the SSourceControlSubmitWidget that owns our list */
    TWeakPtr< SGitHubToolsPullRequestReview > SourceControlSubmitWidgetPtr;

    /** The item associated with this row of data */
    FGithubToolsPullRequestFileInfosPtr FileInfos;
};

#undef LOCTEXT_NAMESPACE