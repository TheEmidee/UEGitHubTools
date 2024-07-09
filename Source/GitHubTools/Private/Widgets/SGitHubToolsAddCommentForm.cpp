#include "SGitHubToolsAddCommentForm.h"

#include "GitHubTools.h"
#include "HttpRequests/GitHubToolsHttpRequest_AddPRReview.h"
#include "HttpRequests/GitHubToolsHttpRequest_AddPRReviewThread.h"
#include "HttpRequests/GitHubToolsHttpRequest_AddPRReviewThreadReply.h"
#include "HttpRequests/GitHubToolsHttpRequest_SubmitPRReview.h"

#include <Widgets/Input/SMultiLineEditableTextBox.h>
#include <Widgets/Layout/SUniformGridPanel.h>

#if SOURCE_CONTROL_WITH_SLATE

#define LOCTEXT_NAMESPACE "SGitHubToolsAddCommentToFile"

SGitHubToolsAddCommentForm::~SGitHubToolsAddCommentForm()
{
}

void SGitHubToolsAddCommentForm::Construct( const FArguments & arguments )
{
    PRInfos = arguments._PRInfos.Get();
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
        FGitHubToolsModule::Get()
            .GetNotificationManager()
            .DisplayModalNotification( LOCTEXT( "SubmitComment", "Submit comment..." ) );

        FGitHubToolsModule::Get()
            .GetRequestManager()
            .SendRequest< FGitHubToolsHttpRequestData_AddPRReviewThreadReply >( ThreadInfos->Id, GetComment() )
            .Then( [ & ]( const TFuture< FGitHubToolsHttpRequestData_AddPRReviewThreadReply > & result ) {
                const auto & result_data = result.Get();
                ThreadInfos->Comments.Add( result_data.GetResult().GetValue() );

                Close();
            } );
    }
    else
    {
        FGitHubToolsModule::Get()
            .GetNotificationManager()
            .DisplayModalNotification( LOCTEXT( "SubmitComment", "Submit comment..." ) );

        FGitHubToolsModule::Get()
            .GetRequestManager()
            .SendRequest< FGitHubToolsHttpRequestData_AddPRReview >( PRInfos->Id )
            .Then( [ & ]( const TFuture< FGitHubToolsHttpRequestData_AddPRReview > & request_future ) {
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
                    RefreshErrorText( LOCTEXT( "Error_NoThreadId", "Could not get a thread ID" ) );
                    return;
                }

                FGitHubToolsModule::Get()
                    .GetRequestManager()
                    .SendRequest< FGitHubToolsHttpRequestData_AddPRReviewThread >( PRInfos->Id, review_id, FileInfos->Path, GetComment() )
                    .Then( [ &, review_id ]( const TFuture< FGitHubToolsHttpRequestData_AddPRReviewThread > & add_pr_review_thread_result ) {
                        auto add_pr_review_thread_result_data = add_pr_review_thread_result.Get();

                        FGitHubToolsModule::Get()
                            .GetRequestManager()
                            .SendRequest< FGitHubToolsHttpRequestData_SubmitPRReview >( PRInfos->Id, review_id, EGitHubToolsPullRequestReviewEvent::RequestChanges )
                            .Then( [ &, add_pr_review_thread_result_data ]( const TFuture< FGitHubToolsHttpRequestData_SubmitPRReview > & submit_pr_result ) {
                                auto submit_pr_result_data = submit_pr_result.Get();

                                if ( submit_pr_result_data.GetResult().IsSet() && !submit_pr_result_data.GetResult()->IsEmpty() )
                                {
                                    PRInfos->Reviews.Add( add_pr_review_thread_result_data.GetResult().GetValue() );

                                    Close();
                                }
                            } );
                    } );
            } );
    }

    return FReply::Handled();
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