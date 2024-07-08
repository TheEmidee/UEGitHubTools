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
    FGitHubToolsModule::Get()
        .GetNotificationManager()
        .DisplayInProgressNotification( LOCTEXT( "OpenSelectedAssets", "Opening Selected Asset... " ) );

    const auto * settings = GetDefault< UGitHubToolsSettings >();

    const auto action = [ this, callback = OnFileInfosStateChanged ]( const TArray< FGithubToolsPullRequestFileInfosPtr > & file_infos ) {
        callback.Execute( file_infos );
        GitHubToolsUtils::OpenAssets( file_infos );
    };

    TArray< FGithubToolsPullRequestFileInfosPtr > selected_files;
    selected_files.Reserve( TreeView->GetNumItemsSelected() );

    for ( auto selected_tree_item : TreeView->GetSelectedItems() )
    {
        selected_files.Emplace( selected_tree_item->FileInfos );
    }

    if ( settings->bMarkFileViewedAutomatically )
    {
        GitHubToolsUtils::MarkFilesAsViewedAndExecuteCallback( PRInfos->Id, MoveTemp( selected_files ), action );
    }
    else
    {
        action( selected_files );
    }

    FGitHubToolsModule::Get()
        .GetNotificationManager()
        .RemoveInProgressNotification();

    return FReply::Handled();
}

FReply SGitHubToolsMultipleAssetActions::OnMarkAsViewedButtonClicked()
{
    FGitHubToolsModule::Get()
        .GetNotificationManager()
        .DisplayInProgressNotification( LOCTEXT( "MarkSelectedAssetsAsViewed", "Marking selected files as viewed... " ) );

    const auto * settings = GetDefault< UGitHubToolsSettings >();

    const auto action = [ this, callback = OnFileInfosStateChanged ]( const TArray< FGithubToolsPullRequestFileInfosPtr > & file_infos ) {
        callback.Execute( file_infos );
    };

    TArray< FGithubToolsPullRequestFileInfosPtr > selected_files;
    selected_files.Reserve( TreeView->GetNumItemsSelected() );

    for ( auto selected_tree_item : TreeView->GetSelectedItems() )
    {
        selected_files.Emplace( selected_tree_item->FileInfos );
    }

    if ( settings->bMarkFileViewedAutomatically )
    {
        GitHubToolsUtils::MarkFilesAsViewedAndExecuteCallback( PRInfos->Id, MoveTemp( selected_files ), action );
    }
    else
    {
        action( selected_files );
    }

    FGitHubToolsModule::Get()
        .GetNotificationManager()
        .RemoveInProgressNotification();

    return FReply::Handled();
}

FReply SGitHubToolsMultipleAssetActions::OnDiffAssetButtonClicked()
{
    FGitHubToolsModule::Get()
        .GetNotificationManager()
        .DisplayInProgressNotification( LOCTEXT( "DiffSelectedAssets", "Diffing Selected Asset... " ) );

    const auto * settings = GetDefault< UGitHubToolsSettings >();

    const auto action = [ this, callback = OnFileInfosStateChanged ]( const TArray< FGithubToolsPullRequestFileInfosPtr > & file_infos ) {
        callback.Execute( file_infos );
        GitHubToolsUtils::DiffFilesAgainstOriginStatusBranch( file_infos );
    };

    TArray< FGithubToolsPullRequestFileInfosPtr > selected_files;
    selected_files.Reserve( TreeView->GetNumItemsSelected() );

    for ( auto selected_tree_item : TreeView->GetSelectedItems() )
    {
        selected_files.Emplace( selected_tree_item->FileInfos );
    }

    if ( settings->bMarkFileViewedAutomatically )
    {
        GitHubToolsUtils::MarkFilesAsViewedAndExecuteCallback( PRInfos->Id, MoveTemp( selected_files ), action );
    }
    else
    {
        action( selected_files );
    }

    FGitHubToolsModule::Get()
        .GetNotificationManager()
        .RemoveInProgressNotification();

    return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE
