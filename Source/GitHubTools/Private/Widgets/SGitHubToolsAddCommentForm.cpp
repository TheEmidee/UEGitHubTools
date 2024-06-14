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
    OnSubmitClicked = arguments._OnSubmitClicked;
    OnCancelClicked = arguments._OnCancelClicked;
    
    ChildSlot
        [ SNew( SBorder )
                .BorderImage( FAppStyle::GetBrush( "ToolPanel.GroupBorder" ) )
                    [ SNew( SVerticalBox ) +
                        SVerticalBox::Slot()
                            .AutoHeight()
                                [ SAssignNew( HeaderText, STextBlock ) ] +
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
                                                .IsEnabled( this, &SGitHubToolsAddCommentForm::CanSubmitComment )
                                                .Text( NSLOCTEXT( "SourceControl.SubmitPanel", "OKButton", "Submit" ) )
                                                .OnClicked( OnSubmitClicked ) ] +
                                    SUniformGridPanel::Slot( 1, 0 )
                                        [ SNew( SButton )
                                                .HAlign( HAlign_Center )
                                                .ContentPadding( FAppStyle::GetMargin( "StandardDialog.ContentPadding" ) )
                                                .Text( NSLOCTEXT( "SourceControl.SubmitPanel", "CancelButton", "Cancel" ) )
                                                .OnClicked( OnCancelClicked ) ] ] ] ];
}

void SGitHubToolsAddCommentForm::SetHeaderText( const FText & text )
{
    HeaderText->SetText( text );
}

bool SGitHubToolsAddCommentForm::CanSubmitComment() const
{
    return !ChangeListDescriptionTextCtrl->GetText().IsEmpty();
}

#undef LOCTEXT_NAMESPACE

#endif // SOURCE_CONTROL_WITH_SLATE