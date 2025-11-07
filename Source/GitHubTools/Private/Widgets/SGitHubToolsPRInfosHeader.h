#pragma once

#include "CoreMinimal.h"
#include "GitHubToolsTypes.h"
#include "Widgets/SCompoundWidget.h"

class SButton;

class SGitHubToolsPRHeader : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS( SGitHubToolsPRHeader )
    {}

    SLATE_ATTRIBUTE( FGithubToolsPullRequestInfosPtr, PRInfos )

    SLATE_END_ARGS()

    void Construct( const FArguments & arguments );

private:
    FReply OpenInGitHubClicked();
    FReply OnApprovePRClicked();
    FReply OnRequestChangesClicked();
    FReply OnAbandonReviewClicked();
    FReply OnMergePRClicked();
    EVisibility GetPendingReviewsVisibility() const;

    FGithubToolsPullRequestInfosPtr PRInfos;
    TSharedPtr< SButton > ApprovePRButton;
};