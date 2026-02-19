#pragma once

#include "CoreMinimal.h"
#include "GitHubToolsTypes.h"
#include "SGitHubToolsPRCommentsList.h"
#include "Widgets/SCompoundWidget.h"

class SGitHubToolsPRConversation final : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS( SGitHubToolsPRConversation )
    {}
    SLATE_ATTRIBUTE( FGithubToolsPullRequestInfosPtr, PRInfos )
    SLATE_END_ARGS()

    virtual ~SGitHubToolsPRConversation() override;

    void Construct( const FArguments & arguments );

private:
    FGithubToolsPullRequestInfosPtr PRInfos;
    TSharedPtr< SGitHubToolsPRCommentsList > CommentsList;
};

#undef LOCTEXT_NAMESPACE