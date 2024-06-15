#pragma once

#include "GitHubToolsTypes.h"

#include <CoreMinimal.h>

class SGitHubToolsPRReviewList;
class SGitSourceControlReviewFilesListRow;

class SGitHubToolsPRInfos final : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS( SGitHubToolsPRInfos )
    {}

    SLATE_ATTRIBUTE( FGithubToolsPullRequestInfosPtr, Infos )

    SLATE_END_ARGS()

    virtual ~SGitHubToolsPRInfos() override;

    void Construct( const FArguments & arguments );

private:
    bool IsFileCommentsButtonEnabled() const;
    EVisibility IsWarningPanelVisible() const;
    TSharedRef< ITableRow > OnGenerateRowForList( FGithubToolsPullRequestFileInfosPtr SubmitItemData, const TSharedRef< STableViewBase > & owner_table );
    EVisibility GetItemRowVisibility( FGithubToolsPullRequestFileInfosPtr file_infos ) const;
    EColumnSortMode::Type GetColumnSortMode( const FName column_id ) const;
    void OnColumnSortModeChanged( const EColumnSortPriority::Type sort_priority, const FName & column_id, const EColumnSortMode::Type sort_mode );
    void RequestSort();
    void SortTree();
    void OnSelectedFileChanged( FGithubToolsPullRequestFileInfosPtr selected_item );
    void OnDiffAgainstRemoteStatusBranchSelected( FGithubToolsPullRequestFileInfosPtr selected_item );
    FReply OpenInGitHubClicked();
    void OnShowOnlyUAssetsCheckStateChanged( ECheckBoxState new_state );
    void OnHideOFPACheckStateChanged( ECheckBoxState new_state );

    FGithubToolsPullRequestInfosPtr PRInfos;
    TSharedPtr< SListView< FGithubToolsPullRequestFileInfosPtr > > ListView;
    TSharedPtr< SCheckBox > OnlyShowAssetsCheckBox;
    TSharedPtr< SCheckBox > HideOFPACheckBox;
    TSharedPtr< SGitHubToolsPRReviewList > ReviewList;
    FName SortByColumn;
    EColumnSortMode::Type SortMode = EColumnSortMode::Ascending;
};

class SGitHubToolsFileInfosRow : public SMultiColumnTableRow< FGithubToolsPullRequestFileInfosPtr >
{
public:
    SLATE_BEGIN_ARGS( SGitHubToolsFileInfosRow )
    {}

    SLATE_ARGUMENT( FGithubToolsPullRequestFileInfosPtr, FileInfos )

    SLATE_END_ARGS()

    void Construct( const FArguments & arguments, const TSharedRef< STableViewBase > & owner_table_view );
    TSharedRef< SWidget > GenerateWidgetForColumn( const FName & column_name );

private:
    FGithubToolsPullRequestFileInfosPtr FileInfos;
};

#undef LOCTEXT_NAMESPACE