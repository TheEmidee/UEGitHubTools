#pragma once

#include "CoreMinimal.h"
#include "GitHubToolsTypes.h"
#include "Widgets/SCompoundWidget.h"

class STableViewBase;
class ITableRow;

class SGitHubToolsPRInfosPendingReviews : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS( SGitHubToolsPRInfosPendingReviews )
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