#include "SGitHubToolsPRPendingReviewTableRow.h"

#include "GitHubTools.h"
#include "HttpRequests/GitHubToolsHttpRequest_DeletePullRequestReview.h"
#include "HttpRequests/GitHubToolsHttpRequest_SubmitPRReview.h"
#include "SGitHubToolsPRCommentTableRow.h"

#if SOURCE_CONTROL_WITH_SLATE

#define LOCTEXT_NAMESPACE "SGitHubToolsPRPendingReviewTableRow"

void SGitHubToolsPRPendingReviewTableRow::Construct( const FArguments & arguments, const TSharedRef< STableViewBase > & owner_table_view )
{
    PRInfos = arguments._PRInfos;
    PendingReview = arguments._PendingReview;
    OnReviewStateUpdated = arguments._OnReviewStateUpdated;

    STableRow< FGithubToolsPullRequestPendingReviewInfosPtr >::Construct(
        STableRow< FGithubToolsPullRequestPendingReviewInfosPtr >::FArguments()
            .Content()
                [ SNew( SBorder )
                        .Padding( FMargin( 5.0f ) )
                            [ SNew( SVerticalBox ) +
                                SVerticalBox::Slot()
                                    .FillHeight( 1.0f )
                                        [ SNew( SBox )
                                                [ SNew( SListView< FGithubToolsPullRequestCommentPtr > )
                                                        .ListItemsSource( &PendingReview->Comments )
                                                        .OnGenerateRow( this, &SGitHubToolsPRPendingReviewTableRow::GenerateCommentRow )
                                                        .SelectionMode( ESelectionMode::None ) ] ] +
                                SVerticalBox::Slot()
                                    .AutoHeight()
                                    .HAlign( HAlign_Right )
                                    .Padding( FMargin( 5.0f ) )
                                        [ SNew( SHorizontalBox ) +
                                            SHorizontalBox::Slot()
                                                .AutoWidth()
                                                    [ SNew( SButton )
                                                            .HAlign( EHorizontalAlignment::HAlign_Center )
                                                            .ContentPadding( FMargin( 5.0f ) )
                                                            .OnClicked( this, &SGitHubToolsPRPendingReviewTableRow::OnApproveReviewButtonClicked )
                                                            .Text( LOCTEXT( "PendingReview_Approve", "Approve Review" ) ) ] +
                                            SHorizontalBox::Slot()
                                                .AutoWidth()
                                                    [ SNew( SButton )
                                                            .HAlign( EHorizontalAlignment::HAlign_Center )
                                                            .ContentPadding( FMargin( 5.0f ) )
                                                            .OnClicked( this, &SGitHubToolsPRPendingReviewTableRow::OnAbandonReviewButtonClicked )
                                                            .Text( LOCTEXT( "PendingReview_Abandon", "Abdandon Review" ) ) ] ] ] ],
        owner_table_view );
}

TSharedRef< ITableRow > SGitHubToolsPRPendingReviewTableRow::GenerateCommentRow( FGithubToolsPullRequestCommentPtr item, const TSharedRef< STableViewBase > & owner_table )
{
    return SNew( SGitHubToolsPRCommentTableRow, owner_table )
        .Comment( item );
}

FReply SGitHubToolsPRPendingReviewTableRow::OnApproveReviewButtonClicked()
{
    FGitHubToolsModule::Get().GetNotificationManager().DisplayModalNotification( LOCTEXT( "ApproveReview", "Approving the review" ) );

    FGitHubToolsModule::Get()
        .GetRequestManager()
        .SendRequest< FGitHubToolsHttpRequestData_SubmitPRReview >( PRInfos->Id, PendingReview->Id, EGitHubToolsPullRequestReviewEvent::RequestChanges )
        .Next( [ & ]( const FGitHubToolsHttpRequestData_SubmitPRReview & result ) {
            if ( !result.HasErrorMessage() )
            {
                OnReviewStateUpdated.Execute( PendingReview );
            }

            FGitHubToolsModule::Get().GetNotificationManager().RemoveModalNotification();
        } );

    return FReply::Handled();
}

FReply SGitHubToolsPRPendingReviewTableRow::OnAbandonReviewButtonClicked()
{
    FGitHubToolsModule::Get().GetNotificationManager().DisplayModalNotification( LOCTEXT( "AbandonReview", "Abandoning the review" ) );

    FGitHubToolsModule::Get()
        .GetRequestManager()
        .SendRequest< FGitHubToolsHttpRequest_DeletePullRequestReview >( PendingReview->Id )
        .Next( [ & ]( const FGitHubToolsHttpRequest_DeletePullRequestReview & result ) {
            if ( !result.HasErrorMessage() )
            {
                OnReviewStateUpdated.Execute( PendingReview );
            }

            FGitHubToolsModule::Get().GetNotificationManager().RemoveModalNotification();
        } );

    return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE

#endif // SOURCE_CONTROL_WITH_SLATE