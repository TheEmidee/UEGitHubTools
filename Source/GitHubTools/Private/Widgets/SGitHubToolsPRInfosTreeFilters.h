#pragma once

#include "CoreMinimal.h"
#include "Misc/TextFilterExpressionEvaluator.h"
#include "Widgets/SCompoundWidget.h"

class SCheckBox;

UENUM()
enum class EGitHubToolsConversationFilterStatus : uint8
{
    NoConversations,
    UnResolvedConversations,
    AllConversationsResolved,
    NoFilter
};

struct FGitHubToolsTreeViewFilters
{
    bool bShowOnlyUAssets = false;
    bool bHideDeveloperFolder = true;
    bool bHideOFPA = true;
    bool bShowOnlyModified = false;
    bool bShowOnlyUnViewed = false;
    bool bShowOnlyDismissed = false;
    EGitHubToolsConversationFilterStatus ConversationFilterStatus;
    FTextFilterExpressionEvaluator SearchTextEvaluator = { ETextFilterExpressionEvaluatorMode::BasicString };
};

class SGitHubToolsPRInfosTreeFilters : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS( SGitHubToolsPRInfosTreeFilters )
    {}

    SLATE_ARGUMENT( TSharedPtr< FGitHubToolsTreeViewFilters >, TreeViewFilters )
    SLATE_EVENT( FSimpleDelegate, OnFiltersChanged )
    SLATE_END_ARGS()

    void Construct( const FArguments & arguments );

private:
    void OnShowOnlyUAssetsCheckStateChanged( ECheckBoxState new_state );
    void OnHideOFPACheckStateChanged( ECheckBoxState new_state );
    void OnHideDeveloperFolderCheckStateChanged( ECheckBoxState new_state );
    void OnShowOnlyModifiedFilesCheckStateChanged( ECheckBoxState new_state );
    void OnShowOnlyUnViewedFilesCheckStateChanged( ECheckBoxState new_state );
    void OnShowOnlyDismissedFilesCheckStateChanged( ECheckBoxState new_state );
    void OnFilterTextChanged( const FText & text );
    void OnConversationStatusFilterChanged( TSharedPtr< EGitHubToolsConversationFilterStatus > selected_item, ESelectInfo::Type selection );

    FSimpleDelegate OnFiltersChanged;
    TSharedPtr< FGitHubToolsTreeViewFilters > TreeViewFilters;
    TSharedPtr< SCheckBox > OnlyShowUnViewedFilesCheckbox;
    TSharedPtr< SCheckBox > OnlyShowDismissedFilesCheckbox;
    TArray< TSharedPtr< EGitHubToolsConversationFilterStatus > > ConversationStatusItemsSource;
    TSharedPtr< EGitHubToolsConversationFilterStatus > SelectedConversationStatusFilter;
};