#pragma once

#include "CoreMinimal.h"
#include "GitHubToolsTypes.h"
#include "SGitHubToolsPRInfosTreeFilters.h"
#include "SGitHubToolsPRReviewList.h"
#include "Widgets/SCompoundWidget.h"

class SGitHubToolsPRFilesChanged final : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS( SGitHubToolsPRFilesChanged )
    {}
    SLATE_ATTRIBUTE( FGithubToolsPullRequestInfosPtr, PRInfos )
    SLATE_END_ARGS()

    virtual ~SGitHubToolsPRFilesChanged() override;

    void Construct( const FArguments & arguments );
    int GetSelectedFilesCount() const;

private:
    void ConstructFileInfos();
    void OnGetChildrenForTreeView( FGitHubToolsFileInfosTreeItemPtr tree_item, TArray< FGitHubToolsFileInfosTreeItemPtr > & children );
    TSharedRef< ITableRow > OnGenerateRowForList( FGitHubToolsFileInfosTreeItemPtr tree_item, const TSharedRef< STableViewBase > & owner_table );
    void OnSelectionChanged( TSharedPtr< FGitHubToolsFileInfosTreeItem > selected_item, ESelectInfo::Type Arg );
    EVisibility GetItemRowVisibility( FGithubToolsPullRequestFileInfosPtr file_infos ) const;
    void RecursivelySelectChildren( TArray< FGitHubToolsFileInfosTreeItemPtr > & children, FGitHubToolsFileInfosTreeItemPtr item );
    void OnTreeViewFiltersChanged();
    void ExpandAllTreeItems();
    void CollapseAllTreeItems();
    void SetItemExpansion( FGitHubToolsFileInfosTreeItemPtr tree_item, bool is_expanded );
    void OnFileInfosStateChanged( FGithubToolsPullRequestFileInfosPtr file_infos );
    void OnMultipleFileInfosStateChanged( const TArray< FGithubToolsPullRequestFileInfosPtr > & file_infos );
    void OnShouldRebuildTree() const;

    FGithubToolsPullRequestInfosPtr PRInfos;
    TSharedPtr< STreeView< FGitHubToolsFileInfosTreeItemPtr > > TreeView;
    TArray< FGitHubToolsFileInfosTreeItemPtr > TreeItems;
    TSharedPtr< FGitHubToolsTreeViewFilters > TreeViewFilters;
    TSharedPtr< SGitHubToolsPRReviewList > ReviewList;
};

#undef LOCTEXT_NAMESPACE