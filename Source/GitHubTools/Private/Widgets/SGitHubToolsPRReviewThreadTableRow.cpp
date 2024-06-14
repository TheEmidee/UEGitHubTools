#include "SGitHubToolsPRReviewThreadTableRow.h"

#include "GitHubTools.h"
#include "SGitHubToolsPRCommentTableRow.h"
#include "HttpRequests/GitHubToolsHttpRequest_ResolveReviewThread.h"

#if SOURCE_CONTROL_WITH_SLATE

#define LOCTEXT_NAMESPACE "SGitHubToolsPRReviewThreadTableRow"

void SGitHubToolsPRReviewThreadTableRow::Construct( const FArguments & arguments, const TSharedRef< STableViewBase > & owner_table_view )
{
    ThreadInfos = arguments._ThreadInfos;

    STableRow< FGithubToolsPullRequestReviewThreadInfosPtr >::Construct(
        STableRow< FGithubToolsPullRequestReviewThreadInfosPtr >::FArguments()
            .Content()
                [ SNew( SBorder )
                        .BorderBackgroundColor( this, &SGitHubToolsPRReviewThreadTableRow::GetBorderBackgroundColor )
                        .Padding( FMargin( 10.0f ) )
                            [ SNew( SVerticalBox ) +
                                SVerticalBox::Slot()
                                    .FillHeight( 1.0f )
                                        [ SNew( SBox )
                                                .WidthOverride( 520 )
                                                    [ SAssignNew( CommentsListView, SListView< FGithubToolsPullRequestCommentPtr > )
                                                            .ItemHeight( 50 )
                                                            .ListItemsSource( &ThreadInfos->Comments )
                                                            .OnGenerateRow( this, &SGitHubToolsPRReviewThreadTableRow::GenerateCommentRow )
                                                            .SelectionMode( ESelectionMode::None ) ] ] +
                                SVerticalBox::Slot()
                                    .AutoHeight()
                                    .Padding( FMargin( 5.0f ) )
                                    .HAlign( EHorizontalAlignment::HAlign_Center )
                                        [ SNew( SButton )
                                                .HAlign( EHorizontalAlignment::HAlign_Center )
                                                .ContentPadding( FMargin( 5.0f ) )
                                                .IsEnabled( !ThreadInfos->bIsResolved )
                                                .OnClicked( this, &SGitHubToolsPRReviewThreadTableRow::OnResolveConversationClicked )
                                                .Text( LOCTEXT( "ReviewThread_ResolveButtonText", "Resolve conversation" ) ) ] ] ],
        owner_table_view );
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
        .SendRequest< FGitHubToolsHttpRequestData_ResolveReviewThread, FGitHubToolsHttpResponseData_ResolveReviewThread >( ThreadInfos->Id )
        .Then( [ & ]( const TFuture< FGitHubToolsHttpResponseData_ResolveReviewThread > & result ) {
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

#undef LOCTEXT_NAMESPACE

#endif // SOURCE_CONTROL_WITH_SLATE