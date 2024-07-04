#pragma once

#include <CoreMinimal.h>

struct FGitHubToolsTreeViewFilters
{
    bool bShowOnlyUAssets = false;
    bool bHideOFPA = true;
    bool bShowOnlyModified = false;
    bool bShowOnlyUnViewed = false;
    bool bShowOnlyDismissed = false;
    bool bShowOnlyWithoutResolution = false;
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
    void OnShowOnlyModifiedFilesCheckStateChanged( ECheckBoxState new_state );
    void OnShowOnlyUnViewedFilesCheckStateChanged( ECheckBoxState new_state );
    void OnShowOnlyDismissedFilesCheckStateChanged( ECheckBoxState new_state );
    void OnShowOnlyWithoutResolutionCheckStateChanged( ECheckBoxState new_state );

    FSimpleDelegate OnFiltersChanged;
    TSharedPtr< FGitHubToolsTreeViewFilters > TreeViewFilters;
    TSharedPtr< SCheckBox > OnlyShowUnViewedFilesCheckbox;
    TSharedPtr< SCheckBox > OnlyShowDismissedFilesCheckbox;
};