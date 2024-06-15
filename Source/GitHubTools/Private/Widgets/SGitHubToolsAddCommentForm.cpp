#include "SGitHubToolsAddCommentForm.h"

#include <Widgets/Input/SMultiLineEditableTextBox.h>
#include <Widgets/Layout/SUniformGridPanel.h>

#if SOURCE_CONTROL_WITH_SLATE

#define LOCTEXT_NAMESPACE "SGitHubToolsAddCommentToFile"

SGitHubToolsAddCommentForm::~SGitHubToolsAddCommentForm()
{
}

void SGitHubToolsAddCommentForm::Construct( const FArguments & arguments )
{
    ParentFrame = arguments._ParentWindow.Get();
    PRInfos = arguments._PRInfos.Get();
    ThreadInfos = arguments._ThreadInfos.Get();

    ChildSlot
        [ SNew( SBorder )
                .BorderImage( FAppStyle::GetBrush( "ToolPanel.GroupBorder" ) )
                    [ SNew( SVerticalBox ) +
                        SVerticalBox::Slot()
                            .AutoHeight()
                                [ SAssignNew( HeaderText, STextBlock )
                                        .Text( ThreadInfos.IsValid()
                                                   ? LOCTEXT( "AddCommentHeaderText", "Add comment to thread" )
                                                   : LOCTEXT( "CreateThread", "Create new comments thread" ) ) ] +
                        SVerticalBox::Slot()
                            .FillHeight( 1.0f )
                                [ SNew( SBox )
                                        .HeightOverride( 300 )
                                            [ SAssignNew( CommentTextBox, SMultiLineEditableTextBox )
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
                                                .IsEnabled( this, &SGitHubToolsAddCommentForm::CanSubmitComment )
                                                .Text( NSLOCTEXT( "SourceControl.SubmitPanel", "OKButton", "Submit" ) )
                                                .OnClicked( this, &SGitHubToolsAddCommentForm::OnSubmitButtonClicked ) ] +
                                    SUniformGridPanel::Slot( 1, 0 )
                                        [ SNew( SButton )
                                                .HAlign( HAlign_Center )
                                                .ContentPadding( FAppStyle::GetMargin( "StandardDialog.ContentPadding" ) )
                                                .Text( NSLOCTEXT( "SourceControl.SubmitPanel", "CancelButton", "Cancel" ) )
                                                .OnClicked( this, &SGitHubToolsAddCommentForm::OnCancelButtonClicked ) ] ] ] ];

    ParentFrame.Pin()->SetWidgetToFocusOnActivate( CommentTextBox );
}

bool SGitHubToolsAddCommentForm::CanSubmitComment() const
{
    return !CommentTextBox->GetText().IsEmpty();
}

FReply SGitHubToolsAddCommentForm::OnSubmitButtonClicked()
{
    CloseDialog();
    return FReply::Handled();
}

FReply SGitHubToolsAddCommentForm::OnCancelButtonClicked()
{
    CloseDialog();
    return FReply::Handled();
}

void SGitHubToolsAddCommentForm::CloseDialog()
{
    ParentFrame.Pin()->RequestDestroyWindow();
    /*if ( const auto containing_window = FSlateApplication::Get().FindWidgetWindow( AsShared() );
         containing_window.IsValid() )
    {
        containing_window->RequestDestroyWindow();
    }*/
}

#undef LOCTEXT_NAMESPACE

#endif // SOURCE_CONTROL_WITH_SLATE