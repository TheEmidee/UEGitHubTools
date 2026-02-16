#pragma once

#include "CoreMinimal.h"
#include "GitHubToolsTypes.h"
#include "Widgets/SCompoundWidget.h"

class SGitHubToolsPRCommentsList final : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS( SGitHubToolsPRCommentsList )
    {}
    SLATE_ATTRIBUTE( TArray< FGithubToolsPullRequestCommentPtr >, Comments )
    SLATE_END_ARGS()

    virtual ~SGitHubToolsPRCommentsList() override;

    void Construct( const FArguments & arguments );
    void RefreshComments( const TArray< FGithubToolsPullRequestCommentPtr > & new_comments );

private:
    TSharedRef< ITableRow > GenerateCommentRow( FGithubToolsPullRequestCommentPtr item, const TSharedRef< STableViewBase > & owner_table );
    TArray< FGithubToolsPullRequestCommentPtr > Comments;
    TSharedPtr< SListView< FGithubToolsPullRequestCommentPtr > > CommentsListView;
};

#undef LOCTEXT_NAMESPACE