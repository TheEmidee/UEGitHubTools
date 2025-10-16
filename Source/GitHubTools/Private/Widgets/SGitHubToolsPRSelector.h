#pragma once

#include "CoreMinimal.h"
#include "GitHubToolsTypes.h"
#include "Widgets/SCompoundWidget.h"

class SGitHubToolsPRSelector final : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS( SGitHubToolsPRSelector )
        {
        }

        SLATE_ATTRIBUTE( TArray< FGitHubToolsOpenedPullRequestInfosPtr >, OpenedPRs )
    SLATE_END_ARGS()

    virtual ~SGitHubToolsPRSelector() override;

    void Construct( const FArguments & arguments );

private:
    TSharedRef< SWidget > OnGenerateComboWidget( FGitHubToolsOpenedPullRequestInfosPtr Option );
    void OnComboSelectionChanged( FGitHubToolsOpenedPullRequestInfosPtr NewSelection, ESelectInfo::Type SelectInfo );
    FText GetCurrentItemText() const;
    FReply OnSwitchPRClicked();
    void CheckoutNewPullRequest();

    TArray< FGitHubToolsOpenedPullRequestInfosPtr > OpenedPRs;
    FGitHubToolsOpenedPullRequestInfosPtr CurrentSelection;
};

#undef LOCTEXT_NAMESPACE