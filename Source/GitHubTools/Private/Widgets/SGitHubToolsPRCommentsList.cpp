#include "SGitHubToolsPRCommentsList.h"

#include "SGitHubToolsPRCommentTableRow.h"

#define LOCTEXT_NAMESPACE "SGitHubToolsPRCommentsList"

SGitHubToolsPRCommentsList::~SGitHubToolsPRCommentsList()
{
}

void SGitHubToolsPRCommentsList::Construct( const FArguments & arguments )
{
    Comments = arguments._Comments.Get();

    ChildSlot
        [ SNew( SBox )
                .WidthOverride( 520 )
                .MaxDesiredWidth( 520 )
                    [ SAssignNew( CommentsListView, SListView< FGithubToolsPullRequestCommentPtr > )
                            .SelectionMode( ESelectionMode::None )
                            .ListItemsSource( &Comments )
                            .OnGenerateRow( this, &SGitHubToolsPRCommentsList::GenerateCommentRow ) ] ];
}

void SGitHubToolsPRCommentsList::RefreshComments( const TArray< FGithubToolsPullRequestCommentPtr > & new_comments )
{
    Comments = new_comments;
    CommentsListView->RequestListRefresh();
}

TSharedRef< ITableRow > SGitHubToolsPRCommentsList::GenerateCommentRow( FGithubToolsPullRequestCommentPtr item, const TSharedRef< STableViewBase > & owner_table )
{
    return SNew( SGitHubToolsPRCommentTableRow, owner_table )
        .Comment( item );
}

#undef LOCTEXT_NAMESPACE