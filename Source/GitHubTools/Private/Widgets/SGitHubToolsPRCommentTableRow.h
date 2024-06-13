#pragma once

#include "GitHubToolsTypes.h"

#include <CoreMinimal.h>
#include <Widgets/Views/STableRow.h>

class SGitHubToolsPRCommentTableRow : public STableRow< FGithubToolsPullRequestCommentPtr >
{
public:
    SLATE_BEGIN_ARGS( SGitHubToolsPRCommentTableRow )
    {}

    SLATE_ARGUMENT( FGithubToolsPullRequestCommentPtr, Comment )

    SLATE_END_ARGS()

    void Construct( const FArguments & arguments, const TSharedRef< STableViewBase > & owner_table_view );

private:
    FGithubToolsPullRequestCommentPtr Comment;
};