#pragma once

#include "GitHubToolsTypes.h"

#include <CoreMinimal.h>

class SGitHubToolsPRInfosMessageDisplay : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS( SGitHubToolsPRInfosMessageDisplay ) :
        _PRInfos()
    {}

    SLATE_ATTRIBUTE( FGithubToolsPullRequestInfosPtr, PRInfos )

    SLATE_END_ARGS()

    void Construct( const FArguments & arguments );

private:
    FReply OnOpenPendingReviewsClicked();

    FGithubToolsPullRequestInfosPtr PRInfos;
    TSharedPtr< SWindow > PendingReviewsWindow;
};