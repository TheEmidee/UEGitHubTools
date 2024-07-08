#pragma once

#include "GitHubToolsTypes.h"

#include <CoreMinimal.h>
#include <Widgets/Input/SMultiLineEditableTextBox.h>

DECLARE_DELEGATE( FGitHubToolsOnAddCommentDoneDelegate );

class SGitHubToolsAddCommentForm final : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS( SGitHubToolsAddCommentForm )
    {}
    SLATE_ATTRIBUTE( FGithubToolsPullRequestInfosPtr, PRInfos )
    SLATE_EVENT( FGitHubToolsOnAddCommentDoneDelegate, OnAddCommentDone )
    SLATE_END_ARGS()

    virtual ~SGitHubToolsAddCommentForm() override;
    void Construct( const FArguments & arguments );

    void Update( FGithubToolsPullRequestFileInfosPtr file_infos, FGithubToolsPullRequestReviewThreadInfosPtr thread_infos );

private:
    bool CanSubmitComment() const;
    FReply OnSubmitButtonClicked();
    FReply OnCancelButtonClicked();
    void Close();
    void OnTextChanged( const FText & text );
    void RefreshErrorText( const FText & error_message );
    EVisibility IsErrorPanelVisible() const;
    FString GetComment() const;

    TSharedPtr< SMultiLineEditableTextBox > CommentTextBox;
    TSharedPtr< STextBlock > HeaderText;
    TSharedPtr< SErrorText > ErrorText;
    FGithubToolsPullRequestInfosPtr PRInfos;
    FGithubToolsPullRequestReviewThreadInfosPtr ThreadInfos;
    FGithubToolsPullRequestFileInfosPtr FileInfos;
    FText ErrorTextMessage;
    FGitHubToolsOnAddCommentDoneDelegate OnAddCommentDone;
};