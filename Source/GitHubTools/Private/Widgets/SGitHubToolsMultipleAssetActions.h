#pragma once

#include "GitHubToolsTypes.h"
#include "SGitHubToolsPRInfos.h"

#include <CoreMinimal.h>

class SGitHubToolsMultipleAssetActions final : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS( SGitHubToolsMultipleAssetActions )
    {}
    SLATE_ARGUMENT( TSharedPtr< STreeView< FGitHubToolsFileInfosTreeItemPtr > >, TreeView )
    SLATE_ARGUMENT( FGithubToolsPullRequestInfosPtr, PRInfos )
    SLATE_EVENT( FGitHubToolsPRInfosOnMultipleFileInfosStateChangedDelegate, OnFileInfosStateChanged )

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

    void AssetsAction( const FText & in_progress_notification_text, TFunction< void( const TArray< FGithubToolsPullRequestFileInfosPtr > & ) > && callback ) const;

    TSharedPtr< STreeView< FGitHubToolsFileInfosTreeItemPtr > > TreeView;
    FGithubToolsPullRequestInfosPtr PRInfos;
    FGitHubToolsPRInfosOnMultipleFileInfosStateChangedDelegate OnFileInfosStateChanged;
};