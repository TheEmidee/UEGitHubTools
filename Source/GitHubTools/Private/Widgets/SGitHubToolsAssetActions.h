#pragma once

#include <CoreMinimal.h>

DECLARE_DELEGATE_RetVal( bool, FGitHubToolsReturnBoolDelegate );
DECLARE_DELEGATE_RetVal( FReply, FGitHubToolsOnButtonClickedDelegate );

class SGitHubToolsAssetActions final : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS( SGitHubToolsAssetActions )
    {}

    SLATE_EVENT( FGitHubToolsReturnBoolDelegate, AreAssetActionsEnabled )
    SLATE_EVENT( FGitHubToolsReturnBoolDelegate, IsMarkedAsViewedButtonEnabled )
    SLATE_EVENT( FGitHubToolsReturnBoolDelegate, IsOpenButtonEnabled )
    SLATE_EVENT( FGitHubToolsReturnBoolDelegate, IsDiffButtonEnabled )
    SLATE_EVENT( FGitHubToolsOnButtonClickedDelegate, OnMarkedAsViewedButtonClicked )
    SLATE_EVENT( FGitHubToolsOnButtonClickedDelegate, OnOpenButtonClicked )
    SLATE_EVENT( FGitHubToolsOnButtonClickedDelegate, OnDiffButtonClicked )

    SLATE_END_ARGS()

    void Construct( const FArguments & arguments );

private:
    FGitHubToolsReturnBoolDelegate AreAssetActionsEnabled;
    FGitHubToolsReturnBoolDelegate IsMarkedAsViewedButtonEnabled;
    FGitHubToolsReturnBoolDelegate IsOpenButtonEnabled;
    FGitHubToolsReturnBoolDelegate IsDiffButtonEnabled;
    FGitHubToolsOnButtonClickedDelegate OnMarkedAsViewedButtonClicked;
    FGitHubToolsOnButtonClickedDelegate OnOpenButtonClicked;
    FGitHubToolsOnButtonClickedDelegate OnDiffButtonClicked;
};