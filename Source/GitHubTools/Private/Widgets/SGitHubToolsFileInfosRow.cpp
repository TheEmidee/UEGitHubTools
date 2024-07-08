#include "SGitHubToolsFileInfosRow.h"

#include "AssetToolsModule.h"
#include "GitHubTools.h"
#include "GitHubToolsGitUtils.h"
#include "GitHubToolsSettings.h"
#include "SGitHubToolsAssetActions.h"
#include "SGitHubToolsPRInfos.h"
#include "picosha2.h"

#include <RevisionControlStyle/RevisionControlStyle.h>

#include <string>

#define LOCTEXT_NAMESPACE "GitHubToolsPullRequestReviewWidget"

void SGitHubToolsFileInfosRow::Construct( const FArguments & arguments, const TSharedRef< STableViewBase > & owner_table_view )
{
    OwningPRInfosWidget = arguments._OwningPRInfosWidget;
    TreeItem = arguments._TreeItem;
    PRInfos = arguments._PRInfos;
    OnFileInfosStateChanged = arguments._OnFileInfosStateChanged;

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
                                [ SNew( SGitHubToolsAssetActions )
                                        .AreAssetActionsEnabled( this, &SGitHubToolsFileInfosRow::GetButtonContainerEnable )
                                        .IsOpenButtonEnabled( this, &SGitHubToolsFileInfosRow::IsOpenButtonEnabled )
                                        .IsDiffButtonEnabled( this, &SGitHubToolsFileInfosRow::IsDiffButtonEnabled )
                                        .IsMarkedAsViewedButtonEnabled( this, &SGitHubToolsFileInfosRow::IsMarkedAsViewedButtonEnabled )
                                        .OnOpenButtonClicked( this, &SGitHubToolsFileInfosRow::OnOpenAssetButtonClicked )
                                        .OnMarkedAsViewedButtonClicked( this, &SGitHubToolsFileInfosRow::OnMarkAsViewedButtonClicked )
                                        .OnDiffButtonClicked( this, &SGitHubToolsFileInfosRow::OnDiffAssetButtonClicked ) ] ],
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
        GitHubToolsUtils::MarkFileAsViewedAndExecuteCallback( PRInfos->Id, TreeItem->FileInfos, [ &, callback = OnFileInfosStateChanged ]( FGithubToolsPullRequestFileInfosPtr file_infos ) {
            callback.Execute( file_infos );
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

        const auto action = [ this, callback = OnFileInfosStateChanged ]( FGithubToolsPullRequestFileInfosPtr file_infos ) {
            callback.Execute( file_infos );
            OpenTreeItemAsset();
        };

        if ( settings->bMarkFileViewedAutomatically && TreeItem->FileInfos->ViewedState != EGitHubToolsFileViewedState::Viewed )
        {
            GitHubToolsUtils::MarkFileAsViewedAndExecuteCallback( PRInfos->Id, TreeItem->FileInfos, action );
        }
        else
        {
            action( TreeItem->FileInfos );
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

        const auto action = [ this, callback = OnFileInfosStateChanged ]( FGithubToolsPullRequestFileInfosPtr file_infos ) {
            callback.Execute( file_infos );
            GitHubToolsUtils::DiffFileAgainstOriginStatusBranch( *file_infos );
        };

        if ( settings->bMarkFileViewedAutomatically && TreeItem->FileInfos->ViewedState != EGitHubToolsFileViewedState::Viewed )
        {
            GitHubToolsUtils::MarkFileAsViewedAndExecuteCallback( PRInfos->Id, TreeItem->FileInfos, action );
        }
        else
        {
            action( TreeItem->FileInfos );
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

bool SGitHubToolsFileInfosRow::GetButtonContainerEnable() const
{
    return OwningPRInfosWidget->GetSelectedFilesCount() == 1;
}

#undef LOCTEXT_NAMESPACE
