#pragma once

#include "GitHubToolsTypes.h"
#include "SGitHubToolsPRInfosTreeFilters.h"

#include <CoreMinimal.h>

class SGitHubToolsMultipleAssetActions;
class SGitHubToolsPRReviewList;
class SGitSourceControlReviewFilesListRow;

DECLARE_DELEGATE_OneParam( FGitHubToolsPRInfosOnFileInfosStateChangedDelegate, FGithubToolsPullRequestFileInfosPtr );
DECLARE_DELEGATE_OneParam( FGitHubToolsPRInfosOnMultipleFileInfosStateChangedDelegate, const TArray< FGithubToolsPullRequestFileInfosPtr > & );

class SGitHubToolsPRInfos final : public SCompoundWidget
{
public:
    friend class SGitHubToolsFileInfosRow;

    SLATE_BEGIN_ARGS( SGitHubToolsPRInfos )
    {}

    SLATE_ATTRIBUTE( FGithubToolsPullRequestInfosPtr, Infos )

    SLATE_END_ARGS()

    virtual ~SGitHubToolsPRInfos() override;

    void Construct( const FArguments & arguments );

    int GetSelectedFilesCount() const;

private:
    void ConstructFileInfos();
    bool IsFileCommentsButtonEnabled() const;
    EVisibility IsWarningPanelVisible() const;
    void OnGetChildrenForTreeView( FGitHubToolsFileInfosTreeItemPtr tree_item, TArray< FGitHubToolsFileInfosTreeItemPtr > & children );
    TSharedRef< ITableRow > OnGenerateRowForList( FGitHubToolsFileInfosTreeItemPtr tree_item, const TSharedRef< STableViewBase > & owner_table );
    EVisibility GetItemRowVisibility( FGithubToolsPullRequestFileInfosPtr file_infos ) const;
    void OnSelectedFileChanged( FGitHubToolsFileInfosTreeItemPtr selected_item );
    void SetItemExpansion( FGitHubToolsFileInfosTreeItemPtr tree_item, bool is_expanded );
    EVisibility GetPRReviewListVisibility() const;
    EVisibility GetMessageDisplayVisibility() const;
    void OnShouldRebuildTree() const;
    void OnFileInfosStateChanged( FGithubToolsPullRequestFileInfosPtr file_infos );
    void OnMultipleFileInfosStateChanged( const TArray< FGithubToolsPullRequestFileInfosPtr > & file_infos );
    void OnTreeViewFiltersChanged();
    void ExpandAllTreeItems();
    void CollapseAllTreeItems();

    FGithubToolsPullRequestInfosPtr PRInfos;
    TSharedPtr< STreeView< FGitHubToolsFileInfosTreeItemPtr > > TreeView;
    TSharedPtr< SGitHubToolsPRReviewList > ReviewList;
    TArray< FGitHubToolsFileInfosTreeItemPtr > TreeItems;
    TSharedPtr< FGitHubToolsTreeViewFilters > TreeViewFilters;
};

#undef LOCTEXT_NAMESPACE