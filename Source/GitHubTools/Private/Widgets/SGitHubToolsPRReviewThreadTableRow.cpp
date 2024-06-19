#include "SGitHubToolsPRReviewThreadTableRow.h"

#include "GitHubTools.h"
#include "SGitHubToolsPRCommentTableRow.h"
#include "HttpRequests/GitHubToolsHttpRequest_ResolveReviewThread.h"

#if SOURCE_CONTROL_WITH_SLATE

#define LOCTEXT_NAMESPACE "SGitHubToolsPRReviewThreadTableRow"

void SGitHubToolsPRReviewThreadTableRow::Construct( const FArguments & arguments, const TSharedRef< STableViewBase > & owner_table_view )
{
    ThreadInfos = arguments._ThreadInfos;
    OnAddCommentButtonClicked = arguments._OnAddCommentClicked;

    OwnerTable = owner_table_view;

    STableRow< FGithubToolsPullRequestReviewThreadInfosPtr >::Construct(
        STableRow< FGithubToolsPullRequestReviewThreadInfosPtr >::FArguments()
            .Content()
                [ SNew( SBorder )
                        .BorderBackgroundColor( this, &SGitHubToolsPRReviewThreadTableRow::GetBorderBackgroundColor )
                        .Padding( FMargin( 10.0f ) )
                            [ SNew( SHorizontalBox ) +
                                SHorizontalBox::Slot()
                                    .AutoWidth()
                                    .VAlign( VAlign_Center )
                                        [ SNew( SButton )
                                                .Text( this, &SGitHubToolsPRReviewThreadTableRow::GetCollapsedButtonText )
                                                .OnClicked( this, &SGitHubToolsPRReviewThreadTableRow::OnCollapsedButtonClicked ) ] +
                                SHorizontalBox::Slot()
                                    .FillWidth( 1.0f )
                                        [ SAssignNew( CommentsPanel, SVerticalBox ) +
                                            SVerticalBox::Slot()
                                                .FillHeight( 1.0f )
                                                    [ SNew( SBox )
                                                            .WidthOverride( 520 )
                                                                [ SAssignNew( CommentsListView, SListView< FGithubToolsPullRequestCommentPtr > )
                                                                        .ItemHeight( 50 )
                                                                        .ListItemsSource( &ThreadInfos->Comments )
                                                                        .OnGenerateRow( this, &SGitHubToolsPRReviewThreadTableRow::GenerateCommentRow )
                                                                        .SelectionMode( ESelectionMode::None ) ] ] ] ] ],
        owner_table_view );

    if ( ThreadInfos->bIsResolved )
    {
        CommentsPanel->AddSlot()
            .AutoHeight()
            .Padding( FMargin( 5.0f ) )
                [ SNew( STextBlock )
                        .Text( FText::FromString( FString::Printf( TEXT( "Resolved by : %s" ), *ThreadInfos->ResolvedByUserName ) ) ) ];
    }
    else
    {
        CommentsPanel->AddSlot().AutoHeight().Padding( FMargin( 5.0f ) )
            [ SNew( SHorizontalBox ) +
                SHorizontalBox::Slot()
                    .AutoWidth()
                        [ SNew( SButton )
                                .HAlign( EHorizontalAlignment::HAlign_Center )
                                .ContentPadding( FMargin( 5.0f ) )
                                .IsEnabled( !ThreadInfos->bIsResolved )
                                .OnClicked( OnAddCommentButtonClicked )
                                .Text( LOCTEXT( "ReviewThread_AddCommmentText", "Add Comment" ) ) ] +
                SHorizontalBox::Slot()
                    .FillWidth( 1.0f ) +
                SHorizontalBox::Slot()
                    .AutoWidth()
                        [ SNew( SButton )
                                .HAlign( EHorizontalAlignment::HAlign_Center )
                                .ContentPadding( FMargin( 5.0f ) )
                                .IsEnabled( !ThreadInfos->bIsResolved )
                                .OnClicked( this, &SGitHubToolsPRReviewThreadTableRow::OnResolveConversationClicked )
                                .Text( LOCTEXT( "ReviewThread_ResolveButtonText", "Resolve conversation" ) ) ] ];
    }
}

FSlateColor SGitHubToolsPRReviewThreadTableRow::GetBorderBackgroundColor() const
{
    return ThreadInfos->bIsResolved
               ? FSlateColor( FColor( 0, 255, 0, 255 ) )
               : FSlateColor( FColor( 255, 0, 0, 255 ) );
}

TSharedRef< ITableRow > SGitHubToolsPRReviewThreadTableRow::GenerateCommentRow( FGithubToolsPullRequestCommentPtr item, const TSharedRef< STableViewBase > & owner_table )
{
    return SNew( SGitHubToolsPRCommentTableRow, owner_table )
        .Comment( item );
}

FReply SGitHubToolsPRReviewThreadTableRow::OnResolveConversationClicked()
{
    FGitHubToolsModule::Get()
        .GetRequestManager()
        .SendRequest< FGitHubToolsHttpRequestData_ResolveReviewThread >( ThreadInfos->Id )
        .Then( [ & ]( const TFuture< FGitHubToolsHttpRequestData_ResolveReviewThread > & result ) {
            const auto & response_data = result.Get();
            const auto & error_message = response_data.GetErrorMessage();

            if ( !error_message.IsEmpty() )
            {
                FGitHubToolsModule::Get()
                    .GetNotificationManager()
                    .DisplayFailureNotification( FText::FromString( FString::Printf( TEXT( "Error while resolving the conversation : %s" ), *error_message ) ) );

                return;
            }

            ThreadInfos->bIsResolved = true;
        } );

    return FReply::Handled();
}

FText SGitHubToolsPRReviewThreadTableRow::GetCollapsedButtonText() const
{
    return CommentsPanel->GetVisibility() == EVisibility::Collapsed
               ? LOCTEXT( "ReviewThread_CollapseButton_Expand", "+" )
               : LOCTEXT( "ReviewThread_CollapseButton_Expand", "-" );
}

FReply SGitHubToolsPRReviewThreadTableRow::OnCollapsedButtonClicked()
{
    CommentsPanel->SetVisibility( CommentsPanel->GetVisibility() == EVisibility::Collapsed
                                      ? EVisibility::Visible
                                      : EVisibility::Collapsed );

    OwnerTable->RequestListRefresh();
    return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE

#endif // SOURCE_CONTROL_WITH_SLATE