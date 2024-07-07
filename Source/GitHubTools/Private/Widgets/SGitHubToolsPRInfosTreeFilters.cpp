#include "SGitHubToolsPRInfosTreeFilters.h"

#include "Widgets/Input/SSearchBox.h"

#define LOCTEXT_NAMESPACE "SGitHubToolsPRInfosTreeFilters"

void SGitHubToolsPRInfosTreeFilters::Construct( const FArguments & arguments )
{
    OnFiltersChanged = arguments._OnFiltersChanged;
    TreeViewFilters = arguments._TreeViewFilters;

    ChildSlot
        [ SNew( SBorder )
                [ SNew( SVerticalBox ) +
                    SVerticalBox::Slot()
                        .AutoHeight()
                            [ SNew( SHorizontalBox ) +
                                SHorizontalBox::Slot()
                                    .FillWidth( 1.0f )
                                        [ SNew( SVerticalBox ) +
                                            SVerticalBox::Slot()
                                                .AutoHeight()
                                                    [ SNew( SCheckBox )
                                                            .IsChecked( TreeViewFilters->bShowOnlyUAssets ? ECheckBoxState::Checked : ECheckBoxState::Unchecked )
                                                            .OnCheckStateChanged( this, &SGitHubToolsPRInfosTreeFilters::OnShowOnlyUAssetsCheckStateChanged )
                                                            .Style( FAppStyle::Get(), "Menu.CheckBox" )
                                                            .ToolTipText( LOCTEXT( "OnlyShowUAssetsToolTip", "Toggle whether or not to only show uasset files." ) )
                                                            .Content()
                                                                [ SNew( SHorizontalBox ) +
                                                                    SHorizontalBox::Slot()
                                                                        .Padding( 2.0f, 0.0f, 0.0f, 0.0f )
                                                                            [ SNew( STextBlock )
                                                                                    .Text( LOCTEXT( "OnlyShowUAssets", "Only show uassets" ) ) ] ] ] +
                                            SVerticalBox::Slot()
                                                .AutoHeight()
                                                    [ SNew( SCheckBox )
                                                            .IsChecked( TreeViewFilters->bHideOFPA ? ECheckBoxState::Checked : ECheckBoxState::Unchecked )
                                                            .OnCheckStateChanged( this, &SGitHubToolsPRInfosTreeFilters::OnHideOFPACheckStateChanged )
                                                            .Style( FAppStyle::Get(), "Menu.CheckBox" )
                                                            .ToolTipText( LOCTEXT( "HideOFPAToolTip", "Hide OFPA assets." ) )
                                                            .Content()
                                                                [ SNew( SHorizontalBox ) +
                                                                    SHorizontalBox::Slot()
                                                                        .Padding( 2.0f, 0.0f, 0.0f, 0.0f )
                                                                            [ SNew( STextBlock )
                                                                                    .Text( LOCTEXT( "HideOFPA", "Hide OFPA assets" ) ) ] ] ] ] +
                                SHorizontalBox::Slot()
                                    .FillWidth( 1.0f )
                                        [ SNew( SVerticalBox ) +
                                            SVerticalBox::Slot()
                                                .AutoHeight()
                                                    [ SNew( SCheckBox )
                                                            .IsChecked( TreeViewFilters->bShowOnlyModified ? ECheckBoxState::Checked : ECheckBoxState::Unchecked )
                                                            .OnCheckStateChanged( this, &SGitHubToolsPRInfosTreeFilters::OnShowOnlyModifiedFilesCheckStateChanged )
                                                            .Style( FAppStyle::Get(), "Menu.CheckBox" )
                                                            .ToolTipText( LOCTEXT( "ShowOnlyModifiedFilesToolTip", "Only modified files." ) )
                                                            .Content()
                                                                [ SNew( SHorizontalBox ) +
                                                                    SHorizontalBox::Slot()
                                                                        .Padding( 2.0f, 0.0f, 0.0f, 0.0f )
                                                                            [ SNew( STextBlock )
                                                                                    .Text( LOCTEXT( "ShowOnlyModifiedFiles", "Only modified files" ) ) ] ] ] +
                                            SVerticalBox::Slot()
                                                .AutoHeight()
                                                    [ SAssignNew( OnlyShowUnViewedFilesCheckbox, SCheckBox )
                                                            .IsChecked( TreeViewFilters->bShowOnlyUnViewed ? ECheckBoxState::Checked : ECheckBoxState::Unchecked )
                                                            .OnCheckStateChanged( this, &SGitHubToolsPRInfosTreeFilters::OnShowOnlyUnViewedFilesCheckStateChanged )
                                                            .Style( FAppStyle::Get(), "Menu.CheckBox" )
                                                            .ToolTipText( LOCTEXT( "ShowOnlyUnViewedFilesToolTip", "Only unviewed files." ) )
                                                            .Content()
                                                                [ SNew( SHorizontalBox ) +
                                                                    SHorizontalBox::Slot()
                                                                        .Padding( 2.0f, 0.0f, 0.0f, 0.0f )
                                                                            [ SNew( STextBlock )
                                                                                    .Text( LOCTEXT( "ShowOnlyUnViewedFiles", "Only unviewed files" ) ) ] ] ] +
                                            SVerticalBox::Slot()
                                                .AutoHeight()
                                                    [ SAssignNew( OnlyShowDismissedFilesCheckbox, SCheckBox )
                                                            .IsChecked( TreeViewFilters->bShowOnlyDismissed ? ECheckBoxState::Checked : ECheckBoxState::Unchecked )
                                                            .OnCheckStateChanged( this, &SGitHubToolsPRInfosTreeFilters::OnShowOnlyWithoutResolutionCheckStateChanged )
                                                            .Style( FAppStyle::Get(), "Menu.CheckBox" )
                                                            .ToolTipText( LOCTEXT( "ShowOnlyDismissedFilesToolTip", "Only files that changed since last view." ) )
                                                            .Content()
                                                                [ SNew( SHorizontalBox ) +
                                                                    SHorizontalBox::Slot()
                                                                        .Padding( 2.0f, 0.0f, 0.0f, 0.0f )
                                                                            [ SNew( STextBlock )
                                                                                    .Text( LOCTEXT( "ShowOnlyDismissedFiles", "Only files that changed since last view" ) ) ] ] ] ] +
                                SHorizontalBox::Slot()
                                    .FillWidth( 1.0f )
                                        [ SNew( SVerticalBox ) +
                                            SVerticalBox::Slot()
                                                .AutoHeight()
                                                    [ SNew( SCheckBox )
                                                            .IsChecked( TreeViewFilters->bShowOnlyWithoutResolution ? ECheckBoxState::Checked : ECheckBoxState::Unchecked )
                                                            .OnCheckStateChanged( this, &SGitHubToolsPRInfosTreeFilters::OnShowOnlyWithoutResolutionCheckStateChanged )
                                                            .Style( FAppStyle::Get(), "Menu.CheckBox" )
                                                            .ToolTipText( LOCTEXT( "ShowOnlyWithoutConversationResolution", "Only unresolved files." ) )
                                                            .Content()
                                                                [ SNew( SHorizontalBox ) +
                                                                    SHorizontalBox::Slot()
                                                                        .Padding( 2.0f, 0.0f, 0.0f, 0.0f )
                                                                            [ SNew( STextBlock )
                                                                                    .Text( LOCTEXT( "ShowOnlyFilesWithoutResolution", "Only unresolved files" ) ) ] ] ] ] ] +
                    SVerticalBox::Slot()
                        .AutoHeight()
                        .Padding( FMargin( 5.0f ) )
                            [ SNew( SSearchBox )
                                    .OnTextChanged( this, &SGitHubToolsPRInfosTreeFilters::OnFilterTextChanged ) ] ] ];
}

void SGitHubToolsPRInfosTreeFilters::OnShowOnlyUAssetsCheckStateChanged( ECheckBoxState new_state )
{
    TreeViewFilters->bShowOnlyUAssets = new_state == ECheckBoxState::Checked;
    OnFiltersChanged.Execute();
}

void SGitHubToolsPRInfosTreeFilters::OnHideOFPACheckStateChanged( ECheckBoxState new_state )
{
    TreeViewFilters->bHideOFPA = new_state == ECheckBoxState::Checked;
    OnFiltersChanged.Execute();
}

void SGitHubToolsPRInfosTreeFilters::OnShowOnlyModifiedFilesCheckStateChanged( ECheckBoxState new_state )
{
    TreeViewFilters->bShowOnlyModified = new_state == ECheckBoxState::Checked;
    OnFiltersChanged.Execute();
}

void SGitHubToolsPRInfosTreeFilters::OnShowOnlyUnViewedFilesCheckStateChanged( ECheckBoxState new_state )
{
    TreeViewFilters->bShowOnlyUnViewed = new_state == ECheckBoxState::Checked;

    if ( TreeViewFilters->bShowOnlyUnViewed )
    {
        TreeViewFilters->bShowOnlyDismissed = false;
        OnlyShowDismissedFilesCheckbox->SetIsChecked( false );
    }

    OnFiltersChanged.Execute();
}

void SGitHubToolsPRInfosTreeFilters::OnShowOnlyDismissedFilesCheckStateChanged( ECheckBoxState new_state )
{
    TreeViewFilters->bShowOnlyDismissed = new_state == ECheckBoxState::Checked;

    if ( TreeViewFilters->bShowOnlyUnViewed )
    {
        TreeViewFilters->bShowOnlyDismissed = false;
        OnlyShowUnViewedFilesCheckbox->SetIsChecked( false );
    }

    OnFiltersChanged.Execute();
}

void SGitHubToolsPRInfosTreeFilters::OnShowOnlyWithoutResolutionCheckStateChanged( ECheckBoxState new_state )
{
    TreeViewFilters->bShowOnlyWithoutResolution = new_state == ECheckBoxState::Checked;
    OnFiltersChanged.Execute();
}

void SGitHubToolsPRInfosTreeFilters::OnFilterTextChanged( const FText & text )
{
    TreeViewFilters->SearchTextEvaluator.SetFilterText( text );
    OnFiltersChanged.Execute();
}

#undef LOCTEXT_NAMESPACE