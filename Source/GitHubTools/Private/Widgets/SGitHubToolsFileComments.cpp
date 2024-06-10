#include "SGitHubToolsFileComments.h"

#include "GitHubToolsHttpRequestManager.h"

#include <Interfaces/IHttpResponse.h>
#include <Widgets/Input/SMultiLineEditableTextBox.h>
#include <Widgets/Layout/SUniformGridPanel.h>

#if SOURCE_CONTROL_WITH_SLATE

#define LOCTEXT_NAMESPACE "SGitHubToolsAddCommentToFile"

SGitHubToolsFileComments::~SGitHubToolsFileComments()
{
}

void SGitHubToolsFileComments::Construct( const FArguments & arguments )
{
    ParentFrame = arguments._ParentWindow.Get();
    Item = arguments._Item.Get();

    Comments.Emplace( MakeShared< FText >( FText::FromString( TEXT( "COUCOU" ) ) ) );
    Comments.Emplace( MakeShared< FText >( FText::FromString( TEXT( "COUCOU" ) ) ) );
    Comments.Emplace( MakeShared< FText >( FText::FromString( TEXT( "COUCOU" ) ) ) );
    Comments.Emplace( MakeShared< FText >( FText::FromString( TEXT( "COUCOU" ) ) ) );
    Comments.Emplace( MakeShared< FText >( FText::FromString( TEXT( "COUCOU" ) ) ) );
    Comments.Emplace( MakeShared< FText >( FText::FromString( TEXT( "COUCOU" ) ) ) );
    Comments.Emplace( MakeShared< FText >( FText::FromString( TEXT( "COUCOU" ) ) ) );
    Comments.Emplace( MakeShared< FText >( FText::FromString( TEXT( "COUCOU" ) ) ) );
    Comments.Emplace( MakeShared< FText >( FText::FromString( TEXT( "COUCOU" ) ) ) );
    Comments.Emplace( MakeShared< FText >( FText::FromString( TEXT( "COUCOU" ) ) ) );
    Comments.Emplace( MakeShared< FText >( FText::FromString( TEXT( "COUCOU" ) ) ) );

    TSharedPtr< SVerticalBox > contents;

    ChildSlot
        [ SNew( SBorder )
                .BorderImage( FAppStyle::GetBrush( "ToolPanel.GroupBorder" ) )
                    [ SAssignNew( contents, SVerticalBox ) ] ];

    contents->AddSlot()
        .AutoHeight()
        .Padding( 5.0f )
            [ SNew( STextBlock )
                    .Text( Item.Get()->AssetName ) ];

    TSharedPtr< SHorizontalBox > horizontal_box;

    contents->AddSlot()
        .FillHeight( 1.0f )
        .HAlign( HAlign_Fill )
        .VAlign( VAlign_Fill )
        .Padding( 5.0f )
            [ SAssignNew( horizontal_box, SHorizontalBox ) ];

    horizontal_box->AddSlot()
        .FillWidth( .5f )
        .Padding( FMargin( 5, 0, 5, 5 ) )
            [ SNew( SVerticalBox ) +
                SVerticalBox::Slot()
                    [ SNew( SBox )
                            .WidthOverride( 520 )
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
                                        .IsEnabled( this, &SGitHubToolsFileComments::IsSubmitEnabled )
                                        .Text( NSLOCTEXT( "SourceControl.SubmitPanel", "OKButton", "Submit" ) )
                                        .OnClicked( this, &SGitHubToolsFileComments::SubmitClicked ) ] +
                            SUniformGridPanel::Slot( 1, 0 )
                                [ SNew( SButton )
                                        .HAlign( HAlign_Center )
                                        .ContentPadding( FAppStyle::GetMargin( "StandardDialog.ContentPadding" ) )
                                        .Text( NSLOCTEXT( "SourceControl.SubmitPanel", "CancelButton", "Cancel" ) )
                                        .OnClicked( this, &SGitHubToolsFileComments::CancelClicked ) ] ] ];

    horizontal_box->AddSlot()
        .FillWidth( .5f )
        .Padding( FMargin( 5, 0, 5, 5 ) )
            [ SNew( SBorder )
                    [ SAssignNew( CommentsListView, SListView< TSharedPtr< FText > > )
                            .ItemHeight( 50 )
                            .ListItemsSource( &Comments )
                            .OnGenerateRow( this, &SGitHubToolsFileComments::GenerateItemRow )
                            .SelectionMode( ESelectionMode::None ) ] ];
}

bool SGitHubToolsFileComments::IsSubmitEnabled() const
{
    return true; //! ChangeListDescriptionTextCtrl->GetText().IsEmpty();
}

FReply SGitHubToolsFileComments::SubmitClicked()
{
    /*FGitHubToolsHttpRequest::SendRequest(
        EGitHubToolsRequestType::GET,
        TEXT( "pulls" ),
        FHttpRequestCompleteDelegate::CreateRaw( this, &SGitHubToolsFileComments::OnRequestCompleted ) );*/

    return FReply::Handled();
}

FReply SGitHubToolsFileComments::CancelClicked()
{
    ParentFrame.Pin()->RequestDestroyWindow();
    return FReply::Handled();
}

TSharedRef< ITableRow > SGitHubToolsFileComments::GenerateItemRow( TSharedPtr< FText > item, const TSharedRef< STableViewBase > & owner_table )
{
    return SNew( STableRow< TSharedPtr< FText > >, owner_table )
        [ SNew( SBorder )
                .Padding( 10.0f )
                    [ SNew( STextBlock )
                            .Text( *item )
                            .Justification( ETextJustify::Left )
                            .WrappingPolicy( ETextWrappingPolicy::DefaultWrapping ) ] ];
}

void SGitHubToolsFileComments::OnRequestCompleted( FHttpRequestPtr pRequest, FHttpResponsePtr pResponse, bool connectedSuccessfully )
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

#undef LOCTEXT_NAMESPACE

#endif // SOURCE_CONTROL_WITH_SLATE