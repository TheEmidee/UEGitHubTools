#include "SGitHubToolsAddCommentForm.h"

#include "GitHubTools.h"
#include "HttpRequests/GitHubToolsHttpRequest_PR_AddComment.h"
#include "HttpRequests/GitHubToolsHttpRequest_PR_AddReviewThread.h"
#include "HttpRequests/GitHubToolsHttpRequest_PR_AddReviewThreadReply.h"
#include "HttpRequests/GitHubToolsHttpRequest_PR_CreatePendingReview.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/Notifications/SErrorText.h"
#include "Widgets/Text/STextBlock.h"

#if SOURCE_CONTROL_WITH_SLATE

#define LOCTEXT_NAMESPACE "SGitHubToolsAddCommentToFile"

SGitHubToolsAddCommentForm::~SGitHubToolsAddCommentForm()
{
}

void SGitHubToolsAddCommentForm::Construct( const FArguments & arguments )
{
    PRInfos = arguments._PRInfos.Get();
    FileInfos = arguments._FileInfos.Get();
    LineInfos = arguments._LineInfos.Get();
    OnAddCommentDone = arguments._OnAddCommentDone;

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
                                                    .OnTextChanged( this, &SGitHubToolsAddCommentForm::OnTextChanged )
                                                    .SelectAllTextWhenFocused( false )
                                                    .Text( FText::GetEmpty() )
                                                    .AutoWrapText( true )
                                                    .IsReadOnly( false ) ] ] +
                        SVerticalBox::Slot()
                            .AutoHeight()
                            .Padding( FMargin( 5, 5, 5, 0 ) )
                                [ SNew( SBorder )
                                        .Visibility( this, &SGitHubToolsAddCommentForm::IsErrorPanelVisible )
                                        .Padding( 5 )
                                            [ SAssignNew( ErrorText, SErrorText ) ] ] +
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

    OnTextChanged( FText::GetEmpty() );
    // ParentFrame.Pin()->SetWidgetToFocusOnActivate( CommentTextBox );
}

void SGitHubToolsAddCommentForm::Update( FGithubToolsPullRequestFileInfosPtr file_infos, FGithubToolsPullRequestReviewThreadInfosPtr thread_infos )
{
    FileInfos = file_infos;
    ThreadInfos = thread_infos;
    CommentTextBox->SetText( FText::GetEmpty() );
}

bool SGitHubToolsAddCommentForm::CanSubmitComment() const
{
    return !CommentTextBox->GetText().IsEmpty();
}

FReply SGitHubToolsAddCommentForm::OnSubmitButtonClicked()
{
    if ( ThreadInfos != nullptr )
    {
        AddReplyToReviewThread();
    }
    else if ( FileInfos != nullptr )
    {
        FGitHubToolsModule::Get()
            .GetNotificationManager()
            .DisplayModalNotification( LOCTEXT( "SubmitComment", "Submit comment..." ) );

        if ( FileInfos->PRInfos->PendingReview == nullptr )
        {
            CreatePendingReview();
        }
        else
        {
            CreateReviewThread();
        }
    }
    else
    {
        FGitHubToolsModule::Get()
            .GetNotificationManager()
            .DisplayModalNotification( LOCTEXT( "SubmitComment", "Submit comment..." ) );

        FGitHubToolsModule::Get()
            .GetRequestManager()
            .SendRequest< FGitHubToolsHttpRequest_PR_AddComment >( PRInfos->Id, GetComment() )
            .Then( [ & ]( const TFuture< FGitHubToolsHttpRequest_PR_AddComment > & result ) {
                const auto & result_data = result.Get();
                PRInfos->Comments.Add( result_data.GetResult().GetValue() );

                Close();
            } );
    }
    return FReply::Handled();
}

void SGitHubToolsAddCommentForm::CreatePendingReview()
{
    FGitHubToolsModule::Get()
        .GetRequestManager()
        .SendRequest< FGitHubToolsHttpRequest_PR_CreatePendingReview >( FileInfos->PRInfos->Id )
        .Then( [ this ]( const TFuture< FGitHubToolsHttpRequest_PR_CreatePendingReview > & request_future ) {
            const auto & request = request_future.Get();

            if ( request.HasErrorMessage() )
            {
                RefreshErrorText( FText::FromString( request.GetErrorMessage() ) );
                return;
            }

            const auto result = request_future.Get().GetResult();

            auto review_id = result.Get( TEXT( "" ) );

            if ( review_id.IsEmpty() )
            {
                RefreshErrorText( LOCTEXT( "Error_NoThreadId", "Could not get a PR Reviww ID" ) );
                return;
            }

            FileInfos->PRInfos->CreatePendingReview( review_id );

            CreateReviewThread();
        } );
}

void SGitHubToolsAddCommentForm::AddReplyToReviewThread()
{
    FGitHubToolsModule::Get()
        .GetNotificationManager()
        .DisplayModalNotification( LOCTEXT( "SubmitComment", "Submit comment..." ) );

    FGitHubToolsModule::Get()
        .GetRequestManager()
        .SendRequest< FGitHubToolsHttpRequest_PR_AddReviewThreadReply >( ThreadInfos->Id, GetComment() )
        .Then( [ & ]( const TFuture< FGitHubToolsHttpRequest_PR_AddReviewThreadReply > & result ) {
            const auto & result_data = result.Get();
            ThreadInfos->Comments.Add( result_data.GetResult().GetValue() );

            Close();
        } );
}

void SGitHubToolsAddCommentForm::CreateReviewThread()
{
    if ( LineInfos.Line != INDEX_NONE )
    {
        FGitHubToolsModule::Get()
            .GetRequestManager()
            .SendRequest< FGitHubToolsHttpRequest_PR_AddReviewThreadToLine >( FileInfos->PRInfos->Id, FileInfos->PRInfos->PendingReview->Id, FileInfos->Path, LineInfos.Side, LineInfos.Line, GetComment() )
            .Then( [ & ]( const TFuture< FGitHubToolsHttpRequest_PR_AddReviewThreadToLine > & add_pr_review_thread_result ) {
                auto add_pr_review_thread_result_data = add_pr_review_thread_result.Get();
                FileInfos->AddReview( add_pr_review_thread_result_data.GetResult().GetValue() );
                Close();
            } );
    }
    else
    {
        FGitHubToolsModule::Get()
            .GetRequestManager()
            .SendRequest< FGitHubToolsHttpRequest_PR_AddReviewThreadToFile >( FileInfos->PRInfos->Id, FileInfos->PRInfos->PendingReview->Id, FileInfos->Path, GetComment() )
            .Then( [ & ]( const TFuture< FGitHubToolsHttpRequest_PR_AddReviewThreadToFile > & add_pr_review_thread_result ) {
                auto add_pr_review_thread_result_data = add_pr_review_thread_result.Get();
                FileInfos->AddReview( add_pr_review_thread_result_data.GetResult().GetValue() );
                Close();
            } );
    }
}

FReply SGitHubToolsAddCommentForm::OnCancelButtonClicked()
{
    Close();
    return FReply::Handled();
}

void SGitHubToolsAddCommentForm::Close()
{
    OnAddCommentDone.Execute();

    FGitHubToolsModule::Get().GetNotificationManager().RemoveModalNotification();

    CommentTextBox->SetText( FText::GetEmpty() );
}

void SGitHubToolsAddCommentForm::OnTextChanged( const FText & text )
{
    RefreshErrorText( text.IsEmpty()
                          ? LOCTEXT( "NoTextError", "You must type a comment" )
                          : FText::GetEmpty() );
}

void SGitHubToolsAddCommentForm::RefreshErrorText( const FText & error_message )
{
    ErrorTextMessage = error_message;
    ErrorText->SetError( ErrorTextMessage );
}

EVisibility SGitHubToolsAddCommentForm::IsErrorPanelVisible() const
{
    return ErrorTextMessage.IsEmpty() ? EVisibility::Collapsed : EVisibility::Visible;
}

FString SGitHubToolsAddCommentForm::GetComment() const
{
    auto comment = CommentTextBox->GetText().ToString();
    comment.ReplaceInline( TEXT( "\r\n" ), TEXT( "<br />" ) );
    return comment;
}

#undef LOCTEXT_NAMESPACE

#endif // SOURCE_CONTROL_WITH_SLATE