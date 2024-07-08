#include "SGitHubToolsMultipleAssetActions.h"

#include "SGitHubToolsAssetActions.h"

#define LOCTEXT_NAMESPACE "SGitHubToolsMultipleAssetActions"

void SGitHubToolsMultipleAssetActions::Construct( const FArguments & arguments )
{
    TreeView = arguments._TreeView;

    ChildSlot
        [ SNew( SGitHubToolsAssetActions )
                .AreAssetActionsEnabled( this, &SGitHubToolsMultipleAssetActions::GetButtonContainerEnable )
                .IsOpenButtonEnabled( this, &SGitHubToolsMultipleAssetActions::IsOpenButtonEnabled )
                .IsDiffButtonEnabled( this, &SGitHubToolsMultipleAssetActions::IsDiffButtonEnabled )
                .IsMarkedAsViewedButtonEnabled( this, &SGitHubToolsMultipleAssetActions::IsMarkedAsViewedButtonEnabled )
                .OnOpenButtonClicked( this, &SGitHubToolsMultipleAssetActions::OnOpenAssetButtonClicked )
                .OnMarkedAsViewedButtonClicked( this, &SGitHubToolsMultipleAssetActions::OnMarkAsViewedButtonClicked )
                .OnDiffButtonClicked( this, &SGitHubToolsMultipleAssetActions::OnDiffAssetButtonClicked ) ];
}

bool SGitHubToolsMultipleAssetActions::GetButtonContainerEnable() const
{
    return TreeView->GetNumItemsSelected() > 1;
}

bool SGitHubToolsMultipleAssetActions::IsOpenButtonEnabled() const
{
    return true;
}

bool SGitHubToolsMultipleAssetActions::IsDiffButtonEnabled() const
{
    return true;
}

bool SGitHubToolsMultipleAssetActions::IsMarkedAsViewedButtonEnabled() const
{
    return true;
}

FReply SGitHubToolsMultipleAssetActions::OnOpenAssetButtonClicked()
{
    return FReply::Handled();
}

FReply SGitHubToolsMultipleAssetActions::OnMarkAsViewedButtonClicked()
{
    return FReply::Handled();
}

FReply SGitHubToolsMultipleAssetActions::OnDiffAssetButtonClicked()
{
    return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE
