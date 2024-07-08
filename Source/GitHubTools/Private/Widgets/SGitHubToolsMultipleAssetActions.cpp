#include "SGitHubToolsMultipleAssetActions.h"

#include "GitHubTools.h"
#include "GitHubToolsGitUtils.h"
#include "GitHubToolsSettings.h"
#include "SGitHubToolsAssetActions.h"

#define LOCTEXT_NAMESPACE "SGitHubToolsMultipleAssetActions"

void SGitHubToolsMultipleAssetActions::Construct( const FArguments & arguments )
{
    TreeView = arguments._TreeView;
    PRInfos = arguments._PRInfos;
    OnFileInfosStateChanged = arguments._OnFileInfosStateChanged;

    ChildSlot
        [ SNew( SGitHubToolsAssetActions )
                .AreAssetActionsEnabled( this, &SGitHubToolsMultipleAssetActions::GetButtonContainerEnable )
                .IsOpenButtonEnabled( this, &SGitHubToolsMultipleAssetActions::IsOpenButtonEnabled )
                .IsDiffButtonEnabled( this, &SGitHubToolsMultipleAssetActions::IsDiffButtonEnabled )
                .IsMarkedAsViewedButtonEnabled( this, &SGitHubToolsMultipleAssetActions::IsMarkedAsViewedButtonEnabled )
                .OnOpenButtonClicked( this, &SGitHubToolsMultipleAssetActions::OnOpenAssetButtonClicked )
                .OnMarkedAsViewedButtonClicked( this, &SGitHubToolsMultipleAssetActions::OnMarkAsViewedButtonClicked )
                .OnDiffButtonClicked( this, &SGitHubToolsMultipleAssetActions::OnDiffAssetButtonClicked ) ];
}

bool SGitHubToolsMultipleAssetActions::GetButtonContainerEnable() const
{
    return TreeView->GetNumItemsSelected() > 1;
}

bool SGitHubToolsMultipleAssetActions::IsOpenButtonEnabled() const
{
    return true;
}

bool SGitHubToolsMultipleAssetActions::IsDiffButtonEnabled() const
{
    return true;
}

bool SGitHubToolsMultipleAssetActions::IsMarkedAsViewedButtonEnabled() const
{
    return true;
}

FReply SGitHubToolsMultipleAssetActions::OnOpenAssetButtonClicked()
{
    AssetsAction( LOCTEXT( "OpenSelectedAssets", "Opening Selected Asset... " ), [ this, callback = OnFileInfosStateChanged ]( const TArray< FGithubToolsPullRequestFileInfosPtr > & file_infos ) {
        callback.Execute( file_infos );
        GitHubToolsUtils::OpenAssets( file_infos );
    } );

    return FReply::Handled();
}

FReply SGitHubToolsMultipleAssetActions::OnMarkAsViewedButtonClicked()
{
    AssetsAction( LOCTEXT( "MarkSelectedAssetsAsViewed", "Marking selected files as viewed... " ), [ this, callback = OnFileInfosStateChanged ]( const TArray< FGithubToolsPullRequestFileInfosPtr > & file_infos ) {
        callback.Execute( file_infos );
    } );

    return FReply::Handled();
}

FReply SGitHubToolsMultipleAssetActions::OnDiffAssetButtonClicked()
{
    AssetsAction( LOCTEXT( "DiffSelectedAssets", "Diffing Selected Asset... " ), [ this, callback = OnFileInfosStateChanged ]( const TArray< FGithubToolsPullRequestFileInfosPtr > & file_infos ) {
        callback.Execute( file_infos );
        GitHubToolsUtils::DiffFilesAgainstOriginStatusBranch( file_infos );
    } );

    return FReply::Handled();
}

void SGitHubToolsMultipleAssetActions::AssetsAction( const FText & in_progress_notification_text, TFunction< void( const TArray< FGithubToolsPullRequestFileInfosPtr > & ) > && callback ) const
{
    FGitHubToolsModule::Get()
        .GetNotificationManager()
        .DisplayInProgressNotification( in_progress_notification_text );

    const auto * settings = GetDefault< UGitHubToolsSettings >();

    TArray< FGithubToolsPullRequestFileInfosPtr > selected_files;
    selected_files.Reserve( TreeView->GetNumItemsSelected() );

    for ( auto selected_tree_item : TreeView->GetSelectedItems() )
    {
        selected_files.Emplace( selected_tree_item->FileInfos );
    }

    if ( settings->bMarkFileViewedAutomatically )
    {
        GitHubToolsUtils::MarkFilesAsViewedAndExecuteCallback( PRInfos->Id, MoveTemp( selected_files ), MoveTemp( callback ) );
    }
    else
    {
        callback( selected_files );
    }

    FGitHubToolsModule::Get()
        .GetNotificationManager()
        .RemoveInProgressNotification();
}

#undef LOCTEXT_NAMESPACE
