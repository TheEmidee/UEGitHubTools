#include "SGitHubToolsPRInfosPendingReviews.h"

#include "SGitHubToolsPRPendingReviewTableRow.h"

#if SOURCE_CONTROL_WITH_SLATE

#define LOCTEXT_NAMESPACE "SGitHubToolsPRInfosPendingReviews"

void SGitHubToolsPRInfosPendingReviews::Construct( const FArguments & arguments )
{
    ParentFrame = arguments._ParentWindow.Get();
    PRInfos = arguments._PRInfos.Get();

    ChildSlot
        [ SNew( SBorder )
                .Padding( FMargin( 5 ) )
                    [ SNew( SListView< FGithubToolsPullRequestPendingReviewInfosPtr > )
                            .ListItemsSource( &PRInfos->PendingReviews )
                            .OnGenerateRow( this, &SGitHubToolsPRInfosPendingReviews::GeneratePendingReviewRow )
                            .SelectionMode( ESelectionMode::None ) ] ];
}

TSharedRef< ITableRow > SGitHubToolsPRInfosPendingReviews::GeneratePendingReviewRow( FGithubToolsPullRequestPendingReviewInfosPtr item, const TSharedRef< STableViewBase > & owner_table )
{
    return SNew( SGitHubToolsPRPendingReviewTableRow, owner_table )
        .PRInfos( PRInfos )
        .PendingReview( item )
        .OnReviewStateUpdated( this, &SGitHubToolsPRInfosPendingReviews::OnReviewStateUpdated );
}

void SGitHubToolsPRInfosPendingReviews::OnReviewStateUpdated( FGithubToolsPullRequestPendingReviewInfosPtr review )
{
    PRInfos->PendingReviews.Remove( review );
    review.Reset();

    ParentFrame.Pin()->RequestDestroyWindow();
}

#undef LOCTEXT_NAMESPACE

#endif // SOURCE_CONTROL_WITH_SLATE