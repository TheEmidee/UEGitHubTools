#pragma once

#include "GitHubToolsTypes.h"

#include <CoreMinimal.h>

class SGitHubToolsPRHeader : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS( SGitHubToolsPRHeader ) :
        _PRInfos()
    {}

    SLATE_ATTRIBUTE( FGithubToolsPullRequestInfosPtr, PRInfos )

    SLATE_END_ARGS()

    void Construct( const FArguments & arguments );

private:
    FReply OpenInGitHubClicked();
    FReply OnApprovePRClicked();
    EVisibility GetPendingReviewsVisibility() const;

    FGithubToolsPullRequestInfosPtr PRInfos;
    TSharedPtr< SButton > ApprovePRButton;
};