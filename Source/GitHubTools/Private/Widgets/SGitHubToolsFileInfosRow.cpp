#include "SGitHubToolsFileInfosRow.h"

#include "AssetToolsModule.h"
#include "GitHubTools.h"
#include "GitHubToolsGitUtils.h"
#include "GitHubToolsSettings.h"
#include "HttpRequests/GitHubToolsHttpRequest_MarkFileAsViewed.h"
#include "SGitHubToolsPRInfos.h"
#include "picosha2.h"

#include <RevisionControlStyle/RevisionControlStyle.h>

#include <string>

#define LOCTEXT_NAMESPACE "GitHubToolsPullRequestReviewWidget"

namespace
{
    void MarkFileAsViewedAndExecuteCallback( const FString & pr_id, FGitHubToolsFileInfosTreeItemPtr tree_item, TFunction< void( FGitHubToolsFileInfosTreeItemPtr ) > callback )
    {
        FGitHubToolsModule::Get()
            .GetRequestManager()
            .SendRequest< FGitHubToolsHttpRequest_MarkFileAsViewed >( pr_id, tree_item->FileInfos->Path )
            .Then( [ item = MoveTemp( tree_item ), callback = MoveTemp( callback ) ]( const TFuture< FGitHubToolsHttpRequest_MarkFileAsViewed > & request ) {
                if ( request.Get().GetResult().Get( false ) )
                {
                    item->FileInfos->UpdateViewedState( EGitHubToolsFileViewedState::Viewed );
                    callback( item );
                }
            } );
    }
}

void SGitHubToolsFileInfosRow::Construct( const FArguments & arguments, const TSharedRef< STableViewBase > & owner_table_view )
{
    TreeItem = arguments._TreeItem;
    PRInfos = arguments._PRInfos;
    OnTreeItemStateChanged = arguments._OnTreeItemStateChanged;

    if ( TreeItem->FileInfos != nullptr )
    {
        STableRow< FGitHubToolsFileInfosTreeItemPtr >::Construct(
            STableRow< FGitHubToolsFileInfosTreeItemPtr >::FArguments()
                .Content()
                    [ SNew( SHorizontalBox ) +
                        SHorizontalBox::Slot()
                            .AutoWidth()
                            .HAlign( HAlign_Center )
                            .VAlign( VAlign_Center )
                                [ SNew( SImage )
                                        .Image( FRevisionControlStyleManager::Get().GetBrush( TreeItem->FileInfos->ChangedStateIconName ) )
                                        .ToolTipText( TreeItem->FileInfos->ChangedStateToolTip ) ] +
                        SHorizontalBox::Slot()
                            .AutoWidth()
                            .HAlign( HAlign_Center )
                            .VAlign( VAlign_Center )
                                [ SNew( SImage )
                                        .Image( FRevisionControlStyleManager::Get().GetBrush( TreeItem->FileInfos->ViewedStateIconName ) )
                                        .ToolTipText( TreeItem->FileInfos->ViewedStateToolTip ) ] +
                        SHorizontalBox::Slot()
                            .FillWidth( 1.0f )
                                [ SNew( STextBlock )
                                        .Text( FText::FromString( TreeItem->Path ) ) ] +
                        SHorizontalBox::Slot()
                            .AutoWidth()
                                [ SNew( SHorizontalBox ) +
                                    SHorizontalBox::Slot()
                                        .AutoWidth()
                                            [ SNew( SButton )
                                                    .Content()
                                                        [ SNew( SImage )
                                                                .Image( FSlateIcon( FAppStyle::GetAppStyleSetName(), "Icons.Localization" ).GetIcon() ) ]
                                                    .ToolTipText( LOCTEXT( "MarkAsViewed", "Mark as Viewed" ) )
                                                    .IsEnabled( this, &SGitHubToolsFileInfosRow::IsDiffButtonEnabled )
                                                    .OnClicked( this, &SGitHubToolsFileInfosRow::OnMarkAsViewedButtonClicked ) ] +
                                    SHorizontalBox::Slot()
                                        .AutoWidth()
                                            [ SNew( SButton )
                                                    .Content()
                                                        [ SNew( SImage )
                                                                .Image( FSlateIcon( FAppStyle::GetAppStyleSetName(), "ContentBrowser.ShowInExplorer" ).GetIcon() ) ]
                                                    .ToolTipText( LOCTEXT( "OpenAsset", "Open the asset" ) )
                                                    .IsEnabled( &SGitHubToolsFileInfosRow::IsOpenButtonEnabled )
                                                    .OnClicked( this, &SGitHubToolsFileInfosRow::OnOpenAssetButtonClicked ) ] +
                                    SHorizontalBox::Slot()
                                        .AutoWidth()
                                            [ SNew( SButton )
                                                    .Content()
                                                        [ SNew( SImage )
                                                                .Image( FSlateIcon( FRevisionControlStyleManager::GetStyleSetName(), "RevisionControl.Actions.Diff" ).GetIcon() ) ]
                                                    .ToolTipText( LOCTEXT( "DiffAsset", "Open the diff tool" ) )
                                                    .IsEnabled( this, &SGitHubToolsFileInfosRow::IsDiffButtonEnabled )
                                                    .OnClicked( this, &SGitHubToolsFileInfosRow::OnDiffAssetButtonClicked ) ] ] ],
            owner_table_view );
    }
    else
    {
        STableRow< FGitHubToolsFileInfosTreeItemPtr >::Construct(
            STableRow< FGitHubToolsFileInfosTreeItemPtr >::FArguments()
                .Content()[ SNew( STextBlock ).Text( FText::FromString( TreeItem->Path ) ) ],
            owner_table_view );
    }
}

FReply SGitHubToolsFileInfosRow::OnMarkAsViewedButtonClicked()
{
    FGitHubToolsModule::Get()
        .GetNotificationManager()
        .DisplayInProgressNotification( LOCTEXT( "MarkSelectedAssetsAsViewed", "Marking selected assets as viewed... " ) );

    if ( TreeItem->FileInfos != nullptr )
    {
        MarkFileAsViewedAndExecuteCallback( PRInfos->Id, TreeItem, [ &, callback = OnTreeItemStateChanged ]( FGitHubToolsFileInfosTreeItemPtr /*tree_item*/ ) {
            callback.Execute( TreeItem );
        } );
    }

    FGitHubToolsModule::Get()
        .GetNotificationManager()
        .RemoveInProgressNotification();

    return FReply::Handled();
}

FReply SGitHubToolsFileInfosRow::OnOpenAssetButtonClicked()
{
    if ( TreeItem->FileInfos != nullptr )
    {
        FGitHubToolsModule::Get()
            .GetNotificationManager()
            .DisplayInProgressNotification( LOCTEXT( "OpenSelectedAssets", "Opening Selected Asset... " ) );

        const auto * settings = GetDefault< UGitHubToolsSettings >();

        const auto action = [ this, callback = OnTreeItemStateChanged ]( FGitHubToolsFileInfosTreeItemPtr item ) {
            callback.Execute( TreeItem );
            OpenTreeItemAsset();
        };

        if ( settings->bMarkFileViewedAutomatically && TreeItem->FileInfos->ViewedState != EGitHubToolsFileViewedState::Viewed )
        {
            MarkFileAsViewedAndExecuteCallback( PRInfos->Id, TreeItem, action );
        }
        else
        {
            action( TreeItem );
        }

        FGitHubToolsModule::Get()
            .GetNotificationManager()
            .RemoveInProgressNotification();
    }

    return FReply::Handled();
}

FReply SGitHubToolsFileInfosRow::OnDiffAssetButtonClicked()
{
    if ( TreeItem->FileInfos != nullptr )
    {
        FGitHubToolsModule::Get()
            .GetNotificationManager()
            .DisplayInProgressNotification( LOCTEXT( "OpenSelectedAssets", "Opening Selected Asset... " ) );

        const auto * settings = GetDefault< UGitHubToolsSettings >();

        const auto action = [ this, callback = OnTreeItemStateChanged ]( FGitHubToolsFileInfosTreeItemPtr item ) {
            callback.Execute( TreeItem );
            GitHubToolsUtils::DiffFileAgainstOriginStatusBranch( *item->FileInfos );
        };

        if ( settings->bMarkFileViewedAutomatically && TreeItem->FileInfos->ViewedState != EGitHubToolsFileViewedState::Viewed )
        {
            MarkFileAsViewedAndExecuteCallback( PRInfos->Id, TreeItem, action );
        }
        else
        {
            action( TreeItem );
        }

        FGitHubToolsModule::Get()
            .GetNotificationManager()
            .RemoveInProgressNotification();
    }

    return FReply::Handled();
}

void SGitHubToolsFileInfosRow::OpenTreeItemAsset()
{
    if ( TreeItem->FileInfos == nullptr )
    {
        return;
    }

    if ( !TreeItem->FileInfos->IsUAsset() )
    {
        std::string str( StringCast< ANSICHAR >( *TreeItem->FileInfos->Path ).Get() );
        const auto hash = picosha2::hash256_hex_string( str );

        TStringBuilder< 512 > url;
        url << PRInfos->URL;
        url << TEXT( "/files#diff-" );
        url << hash.data();

        FPlatformProcess::LaunchURL( *url, nullptr, nullptr );

        return;
    }

    const auto asset_data = GitHubToolsUtils::GetAssetDataFromFileInfos( *TreeItem->FileInfos );
    if ( asset_data.IsSet() )
    {
        const auto & asset_tools_module = FModuleManager::GetModuleChecked< FAssetToolsModule >( "AssetTools" );
        asset_tools_module.Get().OpenEditorForAssets( { asset_data.GetValue().GetAsset() } );
    }
}

bool SGitHubToolsFileInfosRow::IsMarkedAsViewedButtonEnabled() const
{
    return TreeItem->FileInfos->ViewedState != EGitHubToolsFileViewedState::Viewed;
}

bool SGitHubToolsFileInfosRow::IsOpenButtonEnabled() const
{
    return TreeItem->FileInfos->ChangedState != EGitHubToolsFileChangedState::Removed;
}

bool SGitHubToolsFileInfosRow::IsDiffButtonEnabled() const
{
    return TreeItem->FileInfos->IsUAsset() &&
           TreeItem->FileInfos->ChangedState == EGitHubToolsFileChangedState::Modified;
}

#undef LOCTEXT_NAMESPACE
