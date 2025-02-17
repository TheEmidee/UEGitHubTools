#pragma once

#include "GitHubToolsTypes.h"

#include <CoreMinimal.h>
#include <Widgets/Input/SMultiLineEditableTextBox.h>

class FGitHubToolsHttpRequestData_AddPRReviewThread;
DECLARE_DELEGATE( FGitHubToolsOnAddCommentDoneDelegate );

struct FGitHubToolsAddCommentLineInfos
{
    FGitHubToolsAddCommentLineInfos() = default;
    FGitHubToolsAddCommentLineInfos( EGitHubToolsDiffSide side, int line ) :
        Side( side ),
        Line( line )
    {
    }

    EGitHubToolsDiffSide Side;
    int Line = INDEX_NONE;
};

class SGitHubToolsAddCommentForm final : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS( SGitHubToolsAddCommentForm )
    {}
    SLATE_ATTRIBUTE( FGithubToolsPullRequestFileInfosPtr, FileInfos )
    SLATE_ATTRIBUTE( FGitHubToolsAddCommentLineInfos, LineInfos )
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
    FGithubToolsPullRequestReviewThreadInfosPtr ThreadInfos;
    FGithubToolsPullRequestFileInfosPtr FileInfos;
    FText ErrorTextMessage;
    FGitHubToolsOnAddCommentDoneDelegate OnAddCommentDone;
    FGitHubToolsAddCommentLineInfos LineInfos = {};
};