#pragma once

#include "GitHubToolsTypes.h"

#include <CoreMinimal.h>

class SGitHubToolsPRInfosPendingReviews : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS( SGitHubToolsPRInfosPendingReviews ) :
        _PRInfos()
    {}

    SLATE_ATTRIBUTE( TSharedPtr< SWindow >, ParentWindow )
    SLATE_ATTRIBUTE( FGithubToolsPullRequestInfosPtr, PRInfos )

    SLATE_END_ARGS()

    void Construct( const FArguments & arguments );

private:
    TSharedRef< ITableRow > GeneratePendingReviewRow( FGithubToolsPullRequestPendingReviewInfosPtr item, const TSharedRef< STableViewBase > & owner_table );
    void OnReviewStateUpdated( FGithubToolsPullRequestPendingReviewInfosPtr review );

    FGithubToolsPullRequestInfosPtr PRInfos;
    TWeakPtr< SWindow > ParentFrame;
};