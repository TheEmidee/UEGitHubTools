#pragma once

#include "CoreMinimal.h"
#include "GitHubToolsTypes.h"
#include "SGitHubToolsPRInfosTreeFilters.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Views/STreeView.h"

class STableViewBase;
class ITableRow;
class SGitHubToolsMultipleAssetActions;
class SGitHubToolsPRReviewList;
class SGitSourceControlReviewFilesListRow;

DECLARE_DELEGATE_OneParam( FGitHubToolsPRInfosOnFileInfosStateChangedDelegate, FGithubToolsPullRequestFileInfosPtr );
DECLARE_DELEGATE_OneParam( FGitHubToolsPRInfosOnMultipleFileInfosStateChangedDelegate, const TArray< FGithubToolsPullRequestFileInfosPtr > & );

class SGitHubToolsPRInfos final : public SCompoundWidget
{
public:
    friend class SGitHubToolsFileInfosRow;

    SLATE_BEGIN_ARGS( SGitHubToolsPRInfos )
    {}
    SLATE_ATTRIBUTE( FGithubToolsPullRequestInfosPtr, Infos )
    SLATE_ATTRIBUTE( TArray< FGitHubToolsOpenedPullRequestInfosPtr >, OpenedPrs )

    SLATE_END_ARGS()

    virtual ~SGitHubToolsPRInfos() override;

    void Construct( const FArguments & arguments );

private:
    bool IsFileCommentsButtonEnabled() const;
    EVisibility IsWarningPanelVisible() const;
    EVisibility GetMessageDisplayVisibility() const;
    TSharedRef< SCheckBox > CreateTabButton( const int tab_index, const FText & text );

    FGithubToolsPullRequestInfosPtr PRInfos;
    TSharedPtr< SWidgetSwitcher > TabSwitcher;
    int32 ActiveTabIndex = 0;
};

#undef LOCTEXT_NAMESPACE