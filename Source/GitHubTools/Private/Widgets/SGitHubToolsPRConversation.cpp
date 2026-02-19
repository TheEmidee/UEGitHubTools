#include "SGitHubToolsPRConversation.h"

#include "SGitHubToolsAddCommentForm.h"
#include "SGitHubToolsPRCommentsList.h"

#define LOCTEXT_NAMESPACE "GitHubToolsPullRequestReviewWidget"

SGitHubToolsPRConversation::~SGitHubToolsPRConversation()
{
}

void SGitHubToolsPRConversation::Construct( const FArguments & arguments )
{
    PRInfos = arguments._PRInfos.Get();

    ChildSlot
        [ SNew( SVerticalBox ) +
            SVerticalBox::Slot()
                .FillHeight( 1.0f )
                    [ SAssignNew( CommentsList, SGitHubToolsPRCommentsList )
                            .Comments( PRInfos->Comments ) ] +
            SVerticalBox::Slot()
                .AutoHeight()
                    [ SNew( SGitHubToolsAddCommentForm )
                            .PRInfos( PRInfos )
                            .OnAddCommentDone_Lambda( [ & ]() {
                                CommentsList->RefreshComments( PRInfos->Comments );
                            } ) ] ];
}

#undef LOCTEXT_NAMESPACE