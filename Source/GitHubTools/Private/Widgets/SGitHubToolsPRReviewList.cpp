#include "SGitHubToolsPRReviewList.h"

#include "GitHubToolsHttpRequestManager.h"
#include "Widgets/SGitHubToolsPRReviewThreadTableRow.h"

#include <Interfaces/IHttpResponse.h>
#include <Widgets/Input/SMultiLineEditableTextBox.h>
#include <Widgets/Layout/SUniformGridPanel.h>

#if SOURCE_CONTROL_WITH_SLATE

#define LOCTEXT_NAMESPACE "SGitHubToolsAddCommentToFile"

SGitHubToolsPRReviewList::~SGitHubToolsPRReviewList()
{
}

void SGitHubToolsPRReviewList::Construct( const FArguments & arguments )
{
    ParentFrame = arguments._ParentWindow.Get();
    PRInfos = arguments._PRInfos.Get();

    TSharedPtr< SVerticalBox > contents;

    ChildSlot
        [ SNew( SBorder )
                .BorderImage( FAppStyle::GetBrush( "ToolPanel.GroupBorder" ) )
                    [ SAssignNew( contents, SVerticalBox ) ] ];

    contents->AddSlot()
        .FillHeight( 1.0f )
        .HAlign( HAlign_Fill )
        .VAlign( VAlign_Fill )
        .Padding( 5.0f )
            [ SNew( SVerticalBox ) +
                SVerticalBox::Slot()
                    .AutoHeight()
                        [ SNew( STextBlock )
                                .Text( LOCTEXT( "CommentsTitle", "Reviews" ) ) ] +
                SVerticalBox::Slot()
                    .AutoHeight()
                        [ SNew( SHorizontalBox ) +
                            SHorizontalBox::Slot()
                                .AutoWidth()
                                .Padding( FMargin( 5 ) )
                                    [ SAssignNew( HideResolvedThreadsCheckBox, SCheckBox )
                                            .ToolTipText( LOCTEXT( "HideResolvedThreads", "Toggle whether or not to hide resolved threads." ) )
                                            .Type( ESlateCheckBoxType::CheckBox )
                                            .IsChecked( ECheckBoxState::Checked )
                                            .OnCheckStateChanged( this, &SGitHubToolsPRReviewList::OnHideResolvedThreadsCheckStateChanged )
                                            .Padding( 4.f )
                                                [ SNew( STextBlock )
                                                        .Text( LOCTEXT( "HideResolvedThreads", "Hide resolved threads" ) ) ] ] ] +
                SVerticalBox::Slot()
                    .FillHeight( 1.0f )
                    .Padding( FMargin( 5.0f ) )
                        [ SNew( SBox )
                                .WidthOverride( 520 )
                                    [ SAssignNew( ReviewThreadsListView, SListView< TSharedPtr< FGithubToolsPullRequestReviewThreadInfos > > )
                                            .ItemHeight( 50 )
                                            .ListItemsSource( &ReviewThreads )
                                            .OnGenerateRow( this, &SGitHubToolsPRReviewList::GenerateItemRow )
                                            .SelectionMode( ESelectionMode::None ) ] ] +
                SVerticalBox::Slot()
                    .AutoHeight()
                        [ SNew( STextBlock )
                                .Text( LOCTEXT( "CommentsAdd", "Add comment" ) ) ] +
                SVerticalBox::Slot()
                    .AutoHeight()
                        [ SNew( SBox )
                                .HeightOverride( 300 )
                                    [ SAssignNew( ChangeListDescriptionTextCtrl, SMultiLineEditableTextBox )
                                            .SelectAllTextWhenFocused( false )
                                            .Text( FText::GetEmpty() )
                                            .AutoWrapText( true )
                                            .IsReadOnly( false ) ] ] +
                SVerticalBox::Slot()
                    .AutoHeight()
                    .HAlign( HAlign_Right )
                    .VAlign( VAlign_Bottom )
                    .Padding( 0.0f, 5.0f, 0.0f, 5.0f )
                        [ SNew( SUniformGridPanel )
                                .SlotPadding( FAppStyle::GetMargin( "StandardDialog.SlotPadding" ) )
                                .MinDesiredSlotWidth( FAppStyle::GetFloat( "StandardDialog.MinDesiredSlotWidth" ) )
                                .MinDesiredSlotHeight( FAppStyle::GetFloat( "StandardDialog.MinDesiredSlotHeight" ) ) +
                            SUniformGridPanel::Slot( 0, 0 )
                                [ SNew( SButton )
                                        .HAlign( HAlign_Center )
                                        .ContentPadding( FAppStyle::GetMargin( "StandardDialog.ContentPadding" ) )
                                        .IsEnabled( this, &SGitHubToolsPRReviewList::IsSubmitEnabled )
                                        .Text( NSLOCTEXT( "SourceControl.SubmitPanel", "OKButton", "Submit" ) )
                                        .OnClicked( this, &SGitHubToolsPRReviewList::SubmitClicked ) ] +
                            SUniformGridPanel::Slot( 1, 0 )
                                [ SNew( SButton )
                                        .HAlign( HAlign_Center )
                                        .ContentPadding( FAppStyle::GetMargin( "StandardDialog.ContentPadding" ) )
                                        .Text( NSLOCTEXT( "SourceControl.SubmitPanel", "CancelButton", "Cancel" ) )
                                        .OnClicked( this, &SGitHubToolsPRReviewList::CancelClicked ) ] ] ];
}

void SGitHubToolsPRReviewList::ShowFileReviews( FGithubToolsPullRequestFileInfosPtr file_infos )
{
    ReviewThreads.Reset();

    for ( const auto review : PRInfos->Reviews )
    {
        if ( review->FileName == file_infos->FileName )
        {
            ReviewThreads.Emplace( review );
        }
    }

    ReviewThreadsListView->RequestListRefresh();
}

bool SGitHubToolsPRReviewList::IsSubmitEnabled() const
{
    return !ChangeListDescriptionTextCtrl->GetText().IsEmpty();
}

FReply SGitHubToolsPRReviewList::SubmitClicked()
{
    /*FGitHubToolsHttpRequest::SendRequest(
        EGitHubToolsRequestType::GET,
        TEXT( "pulls" ),
        FHttpRequestCompleteDelegate::CreateRaw( this, &SGitHubToolsPRReviewList::OnRequestCompleted ) );*/

    return FReply::Handled();
}

FReply SGitHubToolsPRReviewList::CancelClicked()
{
    ParentFrame.Pin()->RequestDestroyWindow();
    return FReply::Handled();
}

TSharedRef< ITableRow > SGitHubToolsPRReviewList::GenerateItemRow( FGithubToolsPullRequestReviewThreadInfosPtr item, const TSharedRef< STableViewBase > & owner_table )
{
    return SNew( SGitHubToolsPRReviewThreadTableRow, owner_table )
        .Visibility( MakeAttributeLambda( [ &, item ]() {
            return ( !item->bIsResolved || HideResolvedThreadsCheckBox->GetCheckedState() == ECheckBoxState::Unchecked )
                       ? EVisibility::Visible
                       : EVisibility::Collapsed;
        } ) )
        .ThreadInfos( item );
}

void SGitHubToolsPRReviewList::OnRequestCompleted( FHttpRequestPtr pRequest, FHttpResponsePtr pResponse, bool connectedSuccessfully )
{
    if ( connectedSuccessfully )
    {
        UE_LOG( LogTemp, Error, TEXT( "%s" ), *pResponse->GetContentAsString() );
        // We should have a JSON response - attempt to process it.
        // ProcessSpaceTrackResponse( pResponse->GetContentAsString() );
    }
    else
    {
        switch ( pRequest->GetStatus() )
        {
            case EHttpRequestStatus::Failed:
                UE_LOG( LogTemp, Error, TEXT( "Connection failed." ) );
            default:
                UE_LOG( LogTemp, Error, TEXT( "Request failed." ) );
        }
    }
}

void SGitHubToolsPRReviewList::OnHideResolvedThreadsCheckStateChanged( ECheckBoxState new_state )
{
    ReviewThreadsListView->RequestListRefresh();
}

#undef LOCTEXT_NAMESPACE

#endif // SOURCE_CONTROL_WITH_SLATE