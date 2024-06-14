#pragma once

#include "GitHubToolsTypes.h"
#include "Interfaces/IHttpRequest.h"

#include <CoreMinimal.h>
#include <Widgets/Input/SMultiLineEditableTextBox.h>

class SGitHubToolsPRReviewList: public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS( SGitHubToolsPRReviewList ) :
        _ParentWindow(),
        _PRInfos()
    {}

    SLATE_ATTRIBUTE( TSharedPtr< SWindow >, ParentWindow )
    SLATE_ATTRIBUTE( FGithubToolsPullRequestInfosPtr, PRInfos )

    SLATE_END_ARGS()

    virtual ~SGitHubToolsPRReviewList() override;

    /** Constructs the widget */
    void Construct( const FArguments & arguments );
    void ShowFileReviews( FGithubToolsPullRequestFileInfosPtr file_infos );

private:
    bool IsSubmitEnabled() const;
    FReply SubmitClicked();
    FReply CancelClicked();
    TSharedRef< ITableRow > GenerateItemRow( FGithubToolsPullRequestReviewThreadInfosPtr item, const TSharedRef< STableViewBase > & owner_table );
    void OnRequestCompleted( FHttpRequestPtr pRequest, FHttpResponsePtr pResponse, bool connectedSuccessfully );
    void OnHideResolvedThreadsCheckStateChanged( ECheckBoxState new_state );

    TWeakPtr< SWindow > ParentFrame;
    FGithubToolsPullRequestInfosPtr PRInfos;
    TSharedPtr< SMultiLineEditableTextBox > ChangeListDescriptionTextCtrl;
    TSharedPtr< SVerticalBox > AllCommentsVerticalBox;
    TSharedPtr< SListView< TSharedPtr< FGithubToolsPullRequestReviewThreadInfos > > > ReviewThreadsListView;
    TArray< TSharedPtr< FGithubToolsPullRequestReviewThreadInfos > > ReviewThreads;
    TSharedPtr< SCheckBox > HideResolvedThreadsCheckBox;
};