#pragma once

#include "CoreMinimal.h"
#include "SGitHubToolsPRInfos.h"
#include "Widgets/Views/STableRow.h"

class SGitHubToolsFileInfosRow final : public STableRow< FGitHubToolsFileInfosTreeItemPtr >
{
public:
    SLATE_BEGIN_ARGS( SGitHubToolsFileInfosRow )
    {}

    SLATE_ARGUMENT( FGitHubToolsFileInfosTreeItemPtr, TreeItem )
    SLATE_ARGUMENT( FGithubToolsPullRequestInfosPtr, PRInfos )
    SLATE_EVENT( FGitHubToolsPRInfosOnFileInfosStateChangedDelegate, OnFileInfosStateChanged )

    SLATE_END_ARGS()

    void Construct( const FArguments & arguments, const TSharedRef< STableViewBase > & owner_table_view );

private:
    FReply OnMarkAsViewedButtonClicked();
    FReply OnOpenAssetButtonClicked();
    FReply OnDiffAssetButtonClicked();
    bool IsMarkedAsViewedButtonEnabled() const;
    bool IsOpenButtonEnabled() const;
    bool IsDiffButtonEnabled() const;
    bool GetButtonContainerEnable() const;

    FGithubToolsPullRequestInfosPtr PRInfos;
    FGitHubToolsFileInfosTreeItemPtr TreeItem;
    FGitHubToolsPRInfosOnFileInfosStateChangedDelegate OnFileInfosStateChanged;
    TSharedPtr< SImage > ReviewImage;
};

#undef LOCTEXT_NAMESPACE