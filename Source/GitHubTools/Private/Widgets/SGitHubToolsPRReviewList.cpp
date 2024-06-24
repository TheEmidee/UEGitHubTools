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
                                                    .IsEnabled( !PRInfos->HasPendingReviews() )
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
                                                    .SelectionMode( ESelectionMode::None ) ] ] ] ];
}

void SGitHubToolsPRReviewList::ShowFileReviews( const FGithubToolsPullRequestFileInfosPtr & file_infos )
{
    FileInfos = file_infos;

    SetEnabled( FileInfos != nullptr );

    ReviewThreads.Reset();

    if ( FileInfos != nullptr )
    {
        for ( const auto review : PRInfos->Reviews )
        {
            if ( review->FileName == file_infos->Path )
            {
                ReviewThreads.Emplace( review );
            }
        }
    }

    ReviewThreadsListView->RequestListRefresh();
}

FReply SGitHubToolsPRReviewList::OnAddCommentClicked( FGithubToolsPullRequestReviewThreadInfosPtr thread_infos )
{
    ShowAddCommentWindow( thread_infos );

    return FReply::Handled();
}

TSharedRef< ITableRow > SGitHubToolsPRReviewList::GenerateItemRow( FGithubToolsPullRequestReviewThreadInfosPtr item, const TSharedRef< STableViewBase > & owner_table )
{
    return SNew( SGitHubToolsPRReviewThreadTableRow, owner_table )
        .EnableButtons( !PRInfos->HasPendingReviews() )
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

FReply SGitHubToolsPRReviewList::OnCreateNewThreadButtonClicked()
{
    ShowAddCommentWindow( nullptr );
    return FReply::Handled();
}

void SGitHubToolsPRReviewList::ShowAddCommentWindow( const FGithubToolsPullRequestReviewThreadInfosPtr & thread_infos )
{
    AddCommentWindow = SNew( SWindow )
                           .Title( LOCTEXT( "SourceControlLoginTitle", "Add comment" ) )
                           .ClientSize( FVector2D( 640, 200 ) )
                           .HasCloseButton( true )
                           .SupportsMaximize( false )
                           .SupportsMinimize( false )
                           .SizingRule( ESizingRule::FixedSize );

    AddCommentWindow->SetOnWindowClosed( FOnWindowClosed::CreateLambda( [ & ]( const TSharedRef< SWindow > & /*window*/ ) {
        AddCommentWindow = nullptr;
        ShowFileReviews( FileInfos );
    } ) );

    AddCommentWindow->SetContent( SNew( SGitHubToolsAddCommentForm )
                                      .ParentWindow( AddCommentWindow )
                                      .PRInfos( PRInfos )
                                      .FileInfos( FileInfos )
                                      .ThreadInfos( thread_infos ) );

    if ( const TSharedPtr< SWindow > root_window = FGlobalTabmanager::Get()->GetRootWindow();
         root_window.IsValid() )
    {
        FSlateApplication::Get().AddModalWindow( AddCommentWindow.ToSharedRef(), root_window );
    }
}

#undef LOCTEXT_NAMESPACE

#endif // SOURCE_CONTROL_WITH_SLATE