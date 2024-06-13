#include "SGitHubToolsPRCommentTableRow.h"

#if SOURCE_CONTROL_WITH_SLATE

#define LOCTEXT_NAMESPACE "SGitHubToolsPRCommentTableRow"

void SGitHubToolsPRCommentTableRow::Construct( const FArguments & arguments, const TSharedRef< STableViewBase > & owner_table_view )
{
    Comment = arguments._Comment;

    STableRow< FGithubToolsPullRequestCommentPtr >::Construct(
        STableRow< FGithubToolsPullRequestCommentPtr >::FArguments()
            .Content()
                [ SNew( SBorder )
                        .Padding( FMargin( 5.0f ) )
                            [ SNew( SVerticalBox ) +
                                SVerticalBox::Slot()
                                    .AutoHeight()
                                        [ SNew( SHorizontalBox ) +
                                            SHorizontalBox::Slot()
                                                .FillWidth( 1.0f )
                                                .HAlign( EHorizontalAlignment::HAlign_Left )
                                                    [ SNew( STextBlock )
                                                            .Text( Comment->Author ) ] +
                                            SHorizontalBox::Slot()
                                                .AutoWidth()
                                                .HAlign( EHorizontalAlignment::HAlign_Right )
                                                    [ SNew( STextBlock )
                                                            .Text( Comment->Date ) ] ] +
                                SVerticalBox::Slot()
                                    .AutoHeight()
                                    .Padding( FMargin( 5.0f ) )
                                        [ SNew( STextBlock )
                                                .Text( Comment->Comment ) ] ] ],
        owner_table_view );

    ///*contents->AddSlot()
    //    .AutoHeight()
    //    .Padding( 5.0f )
    //        [ SNew( STextBlock )
    //                .Text( Item.Get()->AssetName ) ];*/

    //contents->AddSlot()
    //    .FillHeight( 1.0f )
    //    .HAlign( HAlign_Fill )
    //    .VAlign( VAlign_Fill )
    //    .Padding( 5.0f )
    //        [ SNew( SVerticalBox ) +
    //            SVerticalBox::Slot()
    //                .AutoHeight()
    //                    [ SNew( STextBlock )
    //                            .Text( LOCTEXT( "CommentsTitle", "Reviews" ) ) ] +
    //            SVerticalBox::Slot()
    //                .FillHeight( 1.0f )
    //                    [ SNew( SBox )
    //                            .WidthOverride( 520 )
    //                                [ SAssignNew( CommentsListView, SListView< TSharedPtr< FText > > )
    //                                        .ItemHeight( 50 )
    //                                        .ListItemsSource( &Comments )
    //                                        .OnGenerateRow( this, &SGitHubToolsPullRequestReviewList::GenerateItemRow )
    //                                        .SelectionMode( ESelectionMode::None ) ] ] +
    //            SVerticalBox::Slot()
    //                .AutoHeight()
    //                    [ SNew( STextBlock )
    //                            .Text( LOCTEXT( "CommentsAdd", "Add comment" ) ) ] +
    //            SVerticalBox::Slot()
    //                .AutoHeight()
    //                    [ SNew( SBox )
    //                            .HeightOverride( 300 )
    //                                [ SAssignNew( ChangeListDescriptionTextCtrl, SMultiLineEditableTextBox )
    //                                        .SelectAllTextWhenFocused( false )
    //                                        .Text( FText::GetEmpty() )
    //                                        .AutoWrapText( true )
    //                                        .IsReadOnly( false ) ] ] +
    //            SVerticalBox::Slot()
    //                .AutoHeight()
    //                .HAlign( HAlign_Right )
    //                .VAlign( VAlign_Bottom )
    //                .Padding( 0.0f, 5.0f, 0.0f, 5.0f )
    //                    [ SNew( SUniformGridPanel )
    //                            .SlotPadding( FAppStyle::GetMargin( "StandardDialog.SlotPadding" ) )
    //                            .MinDesiredSlotWidth( FAppStyle::GetFloat( "StandardDialog.MinDesiredSlotWidth" ) )
    //                            .MinDesiredSlotHeight( FAppStyle::GetFloat( "StandardDialog.MinDesiredSlotHeight" ) ) +
    //                        SUniformGridPanel::Slot( 0, 0 )
    //                            [ SNew( SButton )
    //                                    .HAlign( HAlign_Center )
    //                                    .ContentPadding( FAppStyle::GetMargin( "StandardDialog.ContentPadding" ) )
    //                                    .IsEnabled( this, &SGitHubToolsPullRequestReviewList::IsSubmitEnabled )
    //                                    .Text( NSLOCTEXT( "SourceControl.SubmitPanel", "OKButton", "Submit" ) )
    //                                    .OnClicked( this, &SGitHubToolsPullRequestReviewList::SubmitClicked ) ] +
    //                        SUniformGridPanel::Slot( 1, 0 )
    //                            [ SNew( SButton )
    //                                    .HAlign( HAlign_Center )
    //                                    .ContentPadding( FAppStyle::GetMargin( "StandardDialog.ContentPadding" ) )
    //                                    .Text( NSLOCTEXT( "SourceControl.SubmitPanel", "CancelButton", "Cancel" ) )
    //                                    .OnClicked( this, &SGitHubToolsPullRequestReviewList::CancelClicked ) ] ] ];
}

#undef LOCTEXT_NAMESPACE

#endif // SOURCE_CONTROL_WITH_SLATE