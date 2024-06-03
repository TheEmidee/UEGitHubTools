#include "SGitHubToolsFileComments.h"

#include <HttpModule.h>
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

    TSharedPtr< SVerticalBox > contents;

    ChildSlot
        [ SNew( SBorder )
                .BorderImage( FAppStyle::GetBrush( "ToolPanel.GroupBorder" ) )
                    [ SAssignNew( contents, SVerticalBox ) ] ];

    contents->AddSlot()
        .AutoHeight()
        .Padding( 5.0f )
            [ SNew( STextBlock )
                    .Text( Item.Get()->GetAssetName() ) ];

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
                            .ItemHeight( 20 )
                            .ListItemsSource( &Comments )
                            .OnGenerateRow( this, &SGitHubToolsFileComments::GenerateItemRow )
                            .SelectionMode( ESelectionMode::None ) ] ];
}

bool SGitHubToolsFileComments::IsSubmitEnabled() const
{
    return !ChangeListDescriptionTextCtrl->GetText().IsEmpty();
}

FReply SGitHubToolsFileComments::SubmitClicked()
{
    /*
     curl
     -L
     -X POST
     -H "Accept: application/vnd.github+json"
     -H "Authorization: Bearer TOKEN"
     -H "X-GitHub-Api-Version: 2022-11-28"
     https://api.github.com/repos/FishingCactus/SummerCamp/pulls/642/comments
     -d '{"body":"Test!","commit_id":"AAAAAAAAA","path":"RELATIVE_PATH","start_line":1,"start_side":"RIGHT","line":2,"side":"RIGHT"}'
     */
    FHttpModule & httpModule = FHttpModule::Get();
    TSharedRef< IHttpRequest, ESPMode::ThreadSafe > Request = httpModule.CreateRequest();
    Request->SetVerb( TEXT( "POST" ) );
    Request->SetHeader( TEXT( "Accept" ), TEXT( "application/json" ) );
    Request->SetHeader( TEXT( "Content-Type" ), TEXT( "application/vnd.github+json" ) );
    Request->SetHeader( TEXT( "Authorization" ), TEXT( "Bearer TOKEN" ) );
    Request->SetHeader( TEXT( "X-GitHub-Api-Version" ), TEXT( "2022-11-28" ) );
    Request->SetURL( TEXT( "https://api.github.com/repos/FishingCactus/SummerCamp/pulls/642/comments" ) );
    Request->SetContentAsString( TEXT( "{\" body \":\" Test !\",\" commit_id \":\" AAAAAAAAA \",\" path \":\" RELATIVE_PATH \", \" subject_type \": \"file\", \" side \":\" RIGHT \"}" ) );

    Request->OnProcessRequestComplete().BindLambda(
        // Here, we "capture" the 'this' pointer (the "&"), so our lambda can call this
        // class's methods in the callback.
        [ & ]( FHttpRequestPtr pRequest, FHttpResponsePtr pResponse, bool connectedSuccessfully ) mutable {
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
                    case EHttpRequestStatus::Failed_ConnectionError:
                        UE_LOG( LogTemp, Error, TEXT( "Connection failed." ) );
                    default:
                        UE_LOG( LogTemp, Error, TEXT( "Request failed." ) );
                }
            }

            ParentFrame.Pin()->RequestDestroyWindow();
        } );

    Request->ProcessRequest();

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
                    [ SNew( STextBlock ).Text( *item ) ] ];
}

#undef LOCTEXT_NAMESPACE

#endif // SOURCE_CONTROL_WITH_SLATE