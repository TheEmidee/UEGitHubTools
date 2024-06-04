#pragma once

#include "GitHubToolsTypes.h"
#include "SGitHubToolsFileComments.h"
#include "Interfaces/IHttpRequest.h"

#include <CoreMinimal.h>
#include <Widgets/Input/SMultiLineEditableTextBox.h>

class SGitHubToolsFileComments : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS( SGitHubToolsFileComments ) :
        _ParentWindow(),
        _Item()
    {}

    SLATE_ATTRIBUTE( TSharedPtr< SWindow >, ParentWindow )
    SLATE_ATTRIBUTE( FGithubToolsPullRequestFileInfosPtr, Item )

    SLATE_END_ARGS()

    virtual ~SGitHubToolsFileComments() override;

    /** Constructs the widget */
    void Construct( const FArguments & arguments );

private:
    bool IsSubmitEnabled() const;
    FReply SubmitClicked();
    FReply CancelClicked();
    TSharedRef< ITableRow > GenerateItemRow( TSharedPtr< FText > item, const TSharedRef< STableViewBase > & owner_table );
    void OnRequestCompleted( FHttpRequestPtr pRequest, FHttpResponsePtr pResponse, bool connectedSuccessfully );

    TWeakPtr< SWindow > ParentFrame;
    FGithubToolsPullRequestFileInfosPtr Item;
    TSharedPtr< SMultiLineEditableTextBox > ChangeListDescriptionTextCtrl;
    TSharedPtr< SVerticalBox > AllCommentsVerticalBox;
    TSharedPtr< SListView< TSharedPtr<  FText > > > CommentsListView;
    TArray< TSharedPtr< FText > > Comments;
};