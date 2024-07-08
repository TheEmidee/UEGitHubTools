#pragma once

#include "GitHubToolsTypes.h"

#include <CoreMinimal.h>

class SGitHubToolsMultipleAssetActions final : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS( SGitHubToolsMultipleAssetActions )
    {}
    SLATE_ARGUMENT( TSharedPtr< STreeView< FGitHubToolsFileInfosTreeItemPtr > >, TreeView )

    SLATE_END_ARGS()

    void Construct( const FArguments & arguments );

private:
    bool GetButtonContainerEnable() const;
    bool IsOpenButtonEnabled() const;
    bool IsDiffButtonEnabled() const;
    bool IsMarkedAsViewedButtonEnabled() const;
    FReply OnOpenAssetButtonClicked();
    FReply OnMarkAsViewedButtonClicked();
    FReply OnDiffAssetButtonClicked();

    TSharedPtr< STreeView< FGitHubToolsFileInfosTreeItemPtr > > TreeView;
};