#pragma once

#include "GitHubToolsReviewFileItem.h"
#include "SGitHubToolsAddCommentToFile.h"

#include <CoreMinimal.h>
#include <Widgets/Input/SMultiLineEditableTextBox.h>

class SGitHubToolsAddCommentToFile : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS( SGitHubToolsAddCommentToFile ) :
        _ParentWindow(),
        _Item()
    {}

    SLATE_ATTRIBUTE( TSharedPtr< SWindow >, ParentWindow )
    SLATE_ATTRIBUTE( TSharedPtr< FGitSourceControlReviewFileItem >, Item )

    SLATE_END_ARGS()

    virtual ~SGitHubToolsAddCommentToFile() override;

    /** Constructs the widget */
    void Construct( const FArguments & arguments );

private:
    bool IsSubmitEnabled() const;
    FReply SubmitClicked();
    FReply CancelClicked();

    TWeakPtr< SWindow > ParentFrame;
    TSharedPtr< FGitSourceControlReviewFileItem > Item;
    TSharedPtr< SMultiLineEditableTextBox > ChangeListDescriptionTextCtrl;
};