#pragma once

#include "SGitHubToolsPRInfos.h"

#include <CoreMinimal.h>

class SGitHubToolsFileInfosRow final : public STableRow< FGitHubToolsFileInfosTreeItemPtr >
{
public:
    SLATE_BEGIN_ARGS( SGitHubToolsFileInfosRow )
    {}

    SLATE_ARGUMENT( FGitHubToolsFileInfosTreeItemPtr, TreeItem )
    SLATE_ARGUMENT( FGithubToolsPullRequestInfosPtr, PRInfos )
    SLATE_EVENT( FGitHubToolsPRInfosOnTreeItemStateChangedDelegate, OnTreeItemStateChanged )

    SLATE_END_ARGS()

    void Construct( const FArguments & arguments, const TSharedRef< STableViewBase > & owner_table_view );

private:
    FReply OnMarkAsViewedButtonClicked();
    FReply OnOpenAssetButtonClicked();
    FReply OnDiffAssetButtonClicked();
    void OpenTreeItemAsset();
    bool IsMarkedAsViewedButtonEnabled() const;
    bool IsOpenButtonEnabled() const;
    bool IsDiffButtonEnabled() const;

    FGithubToolsPullRequestInfosPtr PRInfos;
    FGitHubToolsFileInfosTreeItemPtr TreeItem;
    FGitHubToolsPRInfosOnTreeItemStateChangedDelegate OnTreeItemStateChanged;
};

#undef LOCTEXT_NAMESPACE