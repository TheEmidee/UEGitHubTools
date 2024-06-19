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
    ParentFrame = arguments._ParentWindow.Get();
    PRInfos = arguments._PRInfos.Get();
    FileInfos = arguments._FileInfos.Get();
    ThreadInfos = arguments._ThreadInfos.Get();

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
                                                .OnClicked( this, &SGitHubToolsAddCommentForm::OnSubmitButtonClicked ) ] +
                                    SUniformGridPanel::Slot( 1, 0 )
                                        [ SNew( SButton )
                                                .HAlign( HAlign_Center )
                                                .ContentPadding( FAppStyle::GetMargin( "StandardDialog.ContentPadding" ) )
                                                .Text( NSLOCTEXT( "SourceControl.SubmitPanel", "CancelButton", "Cancel" ) )
                                                .OnClicked( this, &SGitHubToolsAddCommentForm::OnCancelButtonClicked ) ] ] ] ];

    ParentFrame.Pin()->SetWidgetToFocusOnActivate( CommentTextBox );
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
            .GetRequestManager()
            .SendRequest< FGitHubToolsHttpRequestData_AddPRReviewThreadReply >( ThreadInfos->Id, CommentTextBox->GetText().ToString() )
            .Then( [ & ]( const TFuture< FGitHubToolsHttpRequestData_AddPRReviewThreadReply > & result ) {
                const auto & result_data = result.Get();
                ThreadInfos->Comments.Add( result_data.GetResult().GetValue() );

                CloseDialog();
            } );
    }
    else
    {
        FGitHubToolsModule::Get()
            .GetRequestManager()
            .SendRequest< FGitHubToolsHttpRequestData_AddPRReview >( PRInfos->Id )
            .Then( [ & ]( const TFuture< FGitHubToolsHttpRequestData_AddPRReview > & add_pr_review_result ) {
                const auto review_id = add_pr_review_result.Get().GetResult().GetValue();

                FGitHubToolsModule::Get()
                    .GetRequestManager()
                    .SendRequest< FGitHubToolsHttpRequestData_AddPRReviewThread >( PRInfos->Id, review_id, FileInfos->Path, CommentTextBox->GetText().ToString() )
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

                                    CloseDialog();
                                }
                            } );
                    } );
            } );
    }

    return FReply::Handled();
}

FReply SGitHubToolsAddCommentForm::OnCancelButtonClicked()
{
    CloseDialog();
    return FReply::Handled();
}

void SGitHubToolsAddCommentForm::CloseDialog()
{
    ParentFrame.Pin()->RequestDestroyWindow();
    if ( const auto containing_window = FSlateApplication::Get().FindWidgetWindow( AsShared() );
         containing_window.IsValid() )
    {
        containing_window->RequestDestroyWindow();
    }
}

#undef LOCTEXT_NAMESPACE

#endif // SOURCE_CONTROL_WITH_SLATE