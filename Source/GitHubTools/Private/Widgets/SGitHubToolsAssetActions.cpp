#include "SGitHubToolsAssetActions.h"

#include "RevisionControlStyle/RevisionControlStyle.h"

#define LOCTEXT_NAMESPACE "SGitHubToolsAssetActions"

void SGitHubToolsAssetActions::Construct( const FArguments & arguments )
{
    AreAssetActionsEnabled = arguments._AreAssetActionsEnabled;
    IsMarkedAsViewedButtonEnabled = arguments._IsMarkedAsViewedButtonEnabled;
    IsOpenButtonEnabled = arguments._IsOpenButtonEnabled;
    IsDiffButtonEnabled = arguments._IsDiffButtonEnabled;
    OnMarkedAsViewedButtonClicked = arguments._OnMarkedAsViewedButtonClicked;
    OnOpenButtonClicked = arguments._OnOpenButtonClicked;
    OnDiffButtonClicked = arguments._OnDiffButtonClicked;

    ChildSlot
        [ SNew( SHorizontalBox )
                .IsEnabled_Lambda( [ & ]() {
                    return AreAssetActionsEnabled.Execute();
                } ) +
            SHorizontalBox::Slot()
                .AutoWidth()
                    [ SNew( SButton )
                            .Content()
                                [ SNew( SImage )
                                        .Image( FCoreStyle::Get().GetBrush( "Symbols.Check" ) ) ]
                            .ToolTipText( LOCTEXT( "MarkAsViewed", "Mark as Viewed" ) )
                            .IsEnabled_Lambda( [ & ]() {
                                return IsMarkedAsViewedButtonEnabled.Execute();
                            } )
                            .OnClicked( arguments._OnMarkedAsViewedButtonClicked ) ] +
            SHorizontalBox::Slot()
                .AutoWidth()
                    [ SNew( SButton )
                            .Content()
                                [ SNew( SImage )
                                        .Image( FSlateIcon( FAppStyle::GetAppStyleSetName(), "ContentBrowser.ShowInExplorer" ).GetIcon() ) ]
                            .ToolTipText( LOCTEXT( "OpenAsset", "Open the asset" ) )
                            .IsEnabled_Lambda( [ & ]() {
                                return IsOpenButtonEnabled.Execute();
                            } )
                            .OnClicked( arguments._OnOpenButtonClicked ) ] +
            SHorizontalBox::Slot()
                .AutoWidth()
                    [ SNew( SButton )
                            .Content()
                                [ SNew( SImage )
                                        .Image( FCoreStyle::Get().GetBrush( "SourceControl.Actions.Diff" ) ) ]
                            .ToolTipText( LOCTEXT( "DiffAsset", "Open the diff tool" ) )
                            .IsEnabled_Lambda( [ & ]() {
                                return IsDiffButtonEnabled.Execute();
                            } )
                            .OnClicked( arguments._OnDiffButtonClicked ) ] ];
}

#undef LOCTEXT_NAMESPACE
