#include "SGitHubToolsFileInfosRow.h"

#include "GitHubTools.h"
#include "GitHubToolsGitUtils.h"
#include "GitHubToolsSettings.h"
#include "SGitHubToolsAssetActions.h"

#include <RevisionControlStyle/RevisionControlStyle.h>

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
                                        .Image( TreeItem->FileInfos->ChangedStateBrush )
                                        .ToolTipText( TreeItem->FileInfos->ChangedStateToolTip ) ] +
                        SHorizontalBox::Slot()
                            .AutoWidth()
                            .HAlign( HAlign_Center )
                            .VAlign( VAlign_Center )
                                [ SNew( SImage )
                                        .Image( TreeItem->FileInfos->ViewedStateBrush )
                                        .ToolTipText( TreeItem->FileInfos->ViewedStateToolTip ) ] +
                        SHorizontalBox::Slot()
                            .AutoWidth()
                            .HAlign( HAlign_Center )
                            .VAlign( VAlign_Center )
                                [ SNew( SImage )
                                        .Image( FCoreStyle::Get().GetBrush( "Icons.Warning" ) )
                                        .Visibility( TreeItem->FileInfos->bHasUnresolvedConversations ? EVisibility::Visible : EVisibility::Collapsed )
                                        .ToolTipText( LOCTEXT( "UnresolvedComments", "This file has unresolved comments" ) ) ] +
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
            GitHubToolsUtils::OpenAssets( { file_infos } );
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
            GitHubToolsUtils::DiffFileAgainstOriginStatusBranch( file_infos );
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
    return TreeItem->FileInfos->ChangedState == EGitHubToolsFileChangedState::Modified;
}

bool SGitHubToolsFileInfosRow::GetButtonContainerEnable() const
{
    return true;
}

#undef LOCTEXT_NAMESPACE
