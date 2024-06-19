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
    enum class EShowFlags : uint8
    {
        All,
        OnlyModified,
        OnlyUnViewed
    };

    void ConstructFileInfos();
    bool IsFileCommentsButtonEnabled() const;
    EVisibility IsWarningPanelVisible() const;
    void OnGetChildrenForTreeView( FGitHubToolsFileInfosTreeItemPtr tree_item, TArray< FGitHubToolsFileInfosTreeItemPtr > & children );
    TSharedRef< ITableRow > OnGenerateRowForList( FGitHubToolsFileInfosTreeItemPtr tree_item, const TSharedRef< STableViewBase > & owner_table );
    EVisibility GetItemRowVisibility( FGithubToolsPullRequestFileInfosPtr file_infos ) const;
    void OnSelectedFileChanged( FGitHubToolsFileInfosTreeItemPtr selected_item );
    void OnDiffAgainstRemoteStatusBranchSelected( FGitHubToolsFileInfosTreeItemPtr selected_item );
    void OnShowOnlyUAssetsCheckStateChanged( ECheckBoxState new_state );
    void OnHideOFPACheckStateChanged( ECheckBoxState new_state );
    void OnShowOnlyModifiedFilesCheckStateChanged( ECheckBoxState new_state );
    void OnShowOnlyUnViewedFilesCheckStateChanged( ECheckBoxState new_state );
    TSharedRef< SWidget > MakeVisibilityComboMenu( TSharedPtr< SComboButton > owner_combo );
    void OnExpandAllClicked();
    void OnCollapseAllClicked();
    void SetItemExpansion( FGitHubToolsFileInfosTreeItemPtr tree_item, bool is_expanded );

    FGithubToolsPullRequestInfosPtr PRInfos;
    TSharedPtr< STreeView< FGitHubToolsFileInfosTreeItemPtr > > TreeView;
    TSharedPtr< SGitHubToolsPRReviewList > ReviewList;
    TSharedPtr< SComboButton > TreeVisibilitySettingsButton;
    TArray< FGitHubToolsFileInfosTreeItemPtr > TreeItems;
    bool bShowOnlyUAssets = true;
    bool bHideOFPA = true;
    EShowFlags ShowFlags = EShowFlags::All;
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