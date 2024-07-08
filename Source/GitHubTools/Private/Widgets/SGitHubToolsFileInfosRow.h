#pragma once

#include "SGitHubToolsPRInfos.h"

#include <CoreMinimal.h>

class SGitHubToolsFileInfosRow final : public STableRow< FGitHubToolsFileInfosTreeItemPtr >
{
public:
    SLATE_BEGIN_ARGS( SGitHubToolsFileInfosRow )
    {}

    SLATE_ARGUMENT( TSharedPtr< SGitHubToolsPRInfos >, OwningPRInfosWidget );
    SLATE_ARGUMENT( FGitHubToolsFileInfosTreeItemPtr, TreeItem )
    SLATE_ARGUMENT( FGithubToolsPullRequestInfosPtr, PRInfos )
    SLATE_EVENT( FGitHubToolsPRInfosOnFileInfosStateChangedDelegate, OnFileInfosStateChanged )

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
    bool GetButtonContainerEnable() const;

    FGithubToolsPullRequestInfosPtr PRInfos;
    FGitHubToolsFileInfosTreeItemPtr TreeItem;
    FGitHubToolsPRInfosOnFileInfosStateChangedDelegate OnFileInfosStateChanged;
    TSharedPtr< SGitHubToolsPRInfos > OwningPRInfosWidget;
};

#undef LOCTEXT_NAMESPACE