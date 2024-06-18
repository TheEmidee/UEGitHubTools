#pragma once

#include "GitHubToolsTypes.h"

#include <CoreMinimal.h>

class SGitHubToolsPRReviewList;
class SGitSourceControlReviewFilesListRow;

struct FGitHubToolsFileInfosTreeItem
{
    explicit FGitHubToolsFileInfosTreeItem( const FString & path ) :
        Path( path )
    {}

    FString Path;
    FGithubToolsPullRequestFileInfosPtr FileInfos;
    TArray< TSharedPtr< FGitHubToolsFileInfosTreeItem > > Children;
};

typedef TSharedPtr< FGitHubToolsFileInfosTreeItem > FGitHubToolsFileInfosTreeItemPtr;

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
    void ConstructFileInfos();
    bool IsFileCommentsButtonEnabled() const;
    EVisibility IsWarningPanelVisible() const;
    void OnGetChildrenForTreeView( FGitHubToolsFileInfosTreeItemPtr tree_item, TArray< FGitHubToolsFileInfosTreeItemPtr > & children );
    TSharedRef< ITableRow > OnGenerateRowForList( FGitHubToolsFileInfosTreeItemPtr tree_item, const TSharedRef< STableViewBase > & owner_table );
    EVisibility GetItemRowVisibility( FGithubToolsPullRequestFileInfosPtr file_infos ) const;
    EColumnSortMode::Type GetColumnSortMode( const FName column_id ) const;
    void OnColumnSortModeChanged( const EColumnSortPriority::Type sort_priority, const FName & column_id, const EColumnSortMode::Type sort_mode );
    void RequestSort();
    void SortTree();
    void OnSelectedFileChanged( FGitHubToolsFileInfosTreeItemPtr selected_item );
    void OnDiffAgainstRemoteStatusBranchSelected( FGitHubToolsFileInfosTreeItemPtr selected_item );
    FReply OpenInGitHubClicked();
    void OnShowOnlyUAssetsCheckStateChanged( ECheckBoxState new_state );
    void OnHideOFPACheckStateChanged( ECheckBoxState new_state );

    FGithubToolsPullRequestInfosPtr PRInfos;
    TSharedPtr< STreeView< FGitHubToolsFileInfosTreeItemPtr > > TreeView;
    TSharedPtr< SCheckBox > OnlyShowAssetsCheckBox;
    TSharedPtr< SCheckBox > HideOFPACheckBox;
    TSharedPtr< SGitHubToolsPRReviewList > ReviewList;
    TArray< FGitHubToolsFileInfosTreeItemPtr > TreeItems;
    FName SortByColumn;
    EColumnSortMode::Type SortMode = EColumnSortMode::Ascending;
};

class SGitHubToolsFileInfosRow : public STableRow< FGitHubToolsFileInfosTreeItemPtr >
{
public:
    SLATE_BEGIN_ARGS( SGitHubToolsFileInfosRow )
    {}

    SLATE_ARGUMENT( FGitHubToolsFileInfosTreeItemPtr, TreeItem )

    SLATE_END_ARGS()

    void Construct( const FArguments & arguments, const TSharedRef< STableViewBase > & owner_table_view );

private:
    FGitHubToolsFileInfosTreeItemPtr TreeItem;
};

#undef LOCTEXT_NAMESPACE