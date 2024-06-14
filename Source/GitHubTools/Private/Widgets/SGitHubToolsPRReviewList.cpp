#include "SGitHubToolsPRReviewList.h"

#include "Widgets/Layout/SWidgetSwitcher.h"
#include "Widgets/SGitHubToolsAddCommentForm.h"
#include "Widgets/SGitHubToolsPRReviewThreadTableRow.h"

#include <Widgets/Input/SMultiLineEditableTextBox.h>

#if SOURCE_CONTROL_WITH_SLATE

#define LOCTEXT_NAMESPACE "SGitHubToolsAddCommentToFile"

SGitHubToolsPRReviewList::~SGitHubToolsPRReviewList()
{
}

void SGitHubToolsPRReviewList::Construct( const FArguments & arguments )
{
    ParentFrame = arguments._ParentWindow.Get();
    PRInfos = arguments._PRInfos.Get();

    TSharedPtr< SVerticalBox > contents;

    ChildSlot
        [ SNew( SBorder )
                .BorderImage( FAppStyle::GetBrush( "ToolPanel.GroupBorder" ) )
                    [ SAssignNew( WidgetSwitcher, SWidgetSwitcher )
                            .WidgetIndex( 0 ) +
                        SWidgetSwitcher::Slot()
                            [ SNew( SVerticalBox ) +
                                SVerticalBox::Slot()
                                    .AutoHeight()
                                        [ SNew( STextBlock )
                                                .Text( LOCTEXT( "CommentsTitle", "Reviews" ) ) ] +
                                SVerticalBox::Slot()
                                    .AutoHeight()
                                        [ SNew( SHorizontalBox ) +
                                            SHorizontalBox::Slot()
                                                .Padding( FMargin( 5.0f ) )
                                                .AutoWidth()
                                                .HAlign( HAlign_Left )
                                                    [ SNew( SButton )
                                                            .Text( LOCTEXT( "CreateNewThread", "Create new thread" ) )
                                                            .OnClicked( this, &SGitHubToolsPRReviewList::OnCreateNewThreadButtonClicked ) ] +
                                            SHorizontalBox::Slot()
                                                .AutoWidth()
                                                .Padding( FMargin( 5 ) )
                                                    [ SAssignNew( HideResolvedThreadsCheckBox, SCheckBox )
                                                            .ToolTipText( LOCTEXT( "HideResolvedThreads", "Toggle whether or not to hide resolved threads." ) )
                                                            .Type( ESlateCheckBoxType::CheckBox )
                                                            .IsChecked( ECheckBoxState::Checked )
                                                            .OnCheckStateChanged( this, &SGitHubToolsPRReviewList::OnHideResolvedThreadsCheckStateChanged )
                                                            .Padding( 4.f )
                                                                [ SNew( STextBlock )
                                                                        .Text( LOCTEXT( "HideResolvedThreads", "Hide resolved threads" ) ) ] ] ] +
                                SVerticalBox::Slot()
                                    .FillHeight( 1.0f )
                                    .Padding( FMargin( 5.0f ) )
                                        [ SNew( SBox )
                                                .WidthOverride( 520 )
                                                    [ SAssignNew( ReviewThreadsListView, SListView< TSharedPtr< FGithubToolsPullRequestReviewThreadInfos > > )
                                                            .ItemHeight( 50 )
                                                            .ListItemsSource( &ReviewThreads )
                                                            .OnGenerateRow( this, &SGitHubToolsPRReviewList::GenerateItemRow )
                                                            .SelectionMode( ESelectionMode::None ) ] ] ] +
                        SWidgetSwitcher::Slot()
                            [ SAssignNew( AddCommentForm, SGitHubToolsAddCommentForm )
                                    .OnSubmitClicked( this, &SGitHubToolsPRReviewList::OnSubmitCommentCliked )
                                    .OnCancelClicked( this, &SGitHubToolsPRReviewList::OnCancelCommentClicked ) ] ] ];
}

void SGitHubToolsPRReviewList::ShowFileReviews( FGithubToolsPullRequestFileInfosPtr file_infos )
{
    ReviewThreads.Reset();

    for ( const auto review : PRInfos->Reviews )
    {
        if ( review->FileName == file_infos->FileName )
        {
            ReviewThreads.Emplace( review );
        }
    }

    ReviewThreadsListView->RequestListRefresh();
}

FReply SGitHubToolsPRReviewList::OnAddCommentClicked( FGithubToolsPullRequestReviewThreadInfosPtr thread_infos )
{
    AddCommentForm->SetHeaderText( LOCTEXT( "AddCommentHeaderText", "Add comment to thread" ) );
    WidgetSwitcher->SetActiveWidgetIndex( 1 );

    return FReply::Handled();
}

TSharedRef< ITableRow > SGitHubToolsPRReviewList::GenerateItemRow( FGithubToolsPullRequestReviewThreadInfosPtr item, const TSharedRef< STableViewBase > & owner_table )
{
    return SNew( SGitHubToolsPRReviewThreadTableRow, owner_table )
        .Visibility( MakeAttributeLambda( [ &, item ]() {
            return ( !item->bIsResolved || HideResolvedThreadsCheckBox->GetCheckedState() == ECheckBoxState::Unchecked )
                       ? EVisibility::Visible
                       : EVisibility::Collapsed;
        } ) )
        .OnAddCommentClicked( this, &SGitHubToolsPRReviewList::OnAddCommentClicked, item )
        .ThreadInfos( item );
}

void SGitHubToolsPRReviewList::OnHideResolvedThreadsCheckStateChanged( ECheckBoxState new_state )
{
    ReviewThreadsListView->RequestListRefresh();
}

bool SGitHubToolsPRReviewList::CanSubmitComment() const
{
    return !ChangeListDescriptionTextCtrl->GetText().IsEmpty();
}

FReply SGitHubToolsPRReviewList::OnSubmitCommentCliked()
{
    WidgetSwitcher->SetActiveWidgetIndex( 0 );
    return FReply::Handled();
}

FReply SGitHubToolsPRReviewList::OnCancelCommentClicked()
{
    WidgetSwitcher->SetActiveWidgetIndex( 0 );
    return FReply::Handled();
}

FReply SGitHubToolsPRReviewList::OnCreateNewThreadButtonClicked()
{
    AddCommentForm->SetHeaderText( LOCTEXT( "CreateThread", "Create new comments thread" ) );
    WidgetSwitcher->SetActiveWidgetIndex( 1 );
    return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE

#endif // SOURCE_CONTROL_WITH_SLATE