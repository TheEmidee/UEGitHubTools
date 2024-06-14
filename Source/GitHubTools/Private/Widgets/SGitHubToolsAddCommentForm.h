#pragma once

#include <CoreMinimal.h>
#include <Widgets/Input/SMultiLineEditableTextBox.h>

class SGitHubToolsAddCommentForm : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS( SGitHubToolsAddCommentForm )
    {}
        SLATE_EVENT( FOnClicked, OnSubmitClicked )
        SLATE_EVENT( FOnClicked, OnCancelClicked )
    SLATE_END_ARGS()

    virtual ~SGitHubToolsAddCommentForm() override;

    void Construct( const FArguments & arguments );
    void SetHeaderText( const FText & text );

private:
    bool CanSubmitComment() const;

    TSharedPtr< SMultiLineEditableTextBox > ChangeListDescriptionTextCtrl;
    TSharedPtr< STextBlock > HeaderText;
    FOnClicked OnSubmitClicked;
    FOnClicked OnCancelClicked;
};