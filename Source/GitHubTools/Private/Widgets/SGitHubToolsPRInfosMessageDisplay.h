#pragma once

#include "CoreMinimal.h"
#include "GitHubToolsTypes.h"
#include "Widgets/SCompoundWidget.h"

class SGitHubToolsPRInfosMessageDisplay : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS( SGitHubToolsPRInfosMessageDisplay )
    {}

    SLATE_ATTRIBUTE( FGithubToolsPullRequestInfosPtr, PRInfos )

    SLATE_END_ARGS()

    void Construct( const FArguments & arguments );

private:
    FReply OnOpenPendingReviewsClicked();

    FGithubToolsPullRequestInfosPtr PRInfos;
    TSharedPtr< SWindow > PendingReviewsWindow;
};