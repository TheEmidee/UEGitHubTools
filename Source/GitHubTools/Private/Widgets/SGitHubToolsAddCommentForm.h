#pragma once

#include "GitHubToolsTypes.h"

#include <CoreMinimal.h>
#include <Widgets/Input/SMultiLineEditableTextBox.h>

class SGitHubToolsAddCommentForm : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS( SGitHubToolsAddCommentForm )
    {}
    SLATE_ATTRIBUTE( TSharedPtr< SWindow >, ParentWindow )
    SLATE_ATTRIBUTE( FGithubToolsPullRequestInfosPtr, PRInfos )
    SLATE_ATTRIBUTE( FGithubToolsPullRequestFileInfosPtr, FileInfos )
    SLATE_ATTRIBUTE( FGithubToolsPullRequestReviewThreadInfosPtr, ThreadInfos )
    SLATE_END_ARGS()

    virtual ~SGitHubToolsAddCommentForm() override;

    void Construct( const FArguments & arguments );

private:
    bool CanSubmitComment() const;
    FReply OnSubmitButtonClicked();
    FReply OnCancelButtonClicked();
    void CloseDialog();

    TSharedPtr< SMultiLineEditableTextBox > CommentTextBox;
    TSharedPtr< STextBlock > HeaderText;
    TWeakPtr< SWindow > ParentFrame;
    FGithubToolsPullRequestInfosPtr PRInfos;
    FGithubToolsPullRequestReviewThreadInfosPtr ThreadInfos;
    FGithubToolsPullRequestFileInfosPtr FileInfos;
};