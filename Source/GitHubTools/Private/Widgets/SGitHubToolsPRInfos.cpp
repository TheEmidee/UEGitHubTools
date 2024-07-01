#include "SGitHubToolsPRInfos.h"

#include "GitHubTools.h"
#include "GitHubToolsGitUtils.h"
#include "GitHubToolsSettings.h"
#include "HttpRequests/GitHubToolsHttpRequest_MarkFileAsViewed.h"
#include "SGitHubToolsPRInfosHeader.h"
#include "SGitHubToolsPRInfosMessageDisplay.h"
#include "SGitHubToolsPRInfosTreeFilters.h"
#include "SGitHubToolsPRReviewList.h"
#include "picosha2.h"

#include <AssetToolsModule.h>
#include <RevisionControlStyle/RevisionControlStyle.h>

#if SOURCE_CONTROL_WITH_SLATE

#define LOCTEXT_NAMESPACE "GitHubToolsPullRequestReviewWidget"

namespace
{
    void OpenTreeItemAsset( FGitHubToolsFileInfosTreeItemPtr tree_item )
    {
        const auto asset_data = GitHubToolsUtils::GetAssetDataFromFileInfos( *tree_item->FileInfos );
        if ( asset_data.IsSet() )
        {
            const auto & asset_tools_module = FModuleManager::GetModuleChecked< FAssetToolsModule >( "AssetTools" );
            asset_tools_module.Get().OpenEditorForAssets( { asset_data.GetValue().GetAsset() } );
        }
    }

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

SGitHubToolsPRInfos::~SGitHubToolsPRInfos()
{
}

void SGitHubToolsPRInfos::Construct( const FArguments & arguments )
{
    PRInfos = arguments._Infos.Get();

    TreeViewFilters = MakeShared< FGitHubToolsTreeViewFilters >();

    ConstructFileInfos();

    TSharedPtr< SVerticalBox > contents;

    ChildSlot
        [ SNew( SBorder )
                .BorderImage( FAppStyle::GetBrush( "ToolPanel.GroupBorder" ) )
                    [ SAssignNew( contents, SVerticalBox ) ] ];

    contents->AddSlot()
        .Padding( FMargin( 5 ) )
        .AutoHeight()
            [ SNew( SGitHubToolsPRHeader )
                    .PRInfos( PRInfos ) ];

    contents->AddSlot()
        .Padding( FMargin( 5.0f ) )
        .AutoHeight()
            [ SNew( SGitHubToolsPRInfosMessageDisplay )
                    .PRInfos( PRInfos )
                    .Visibility( this, &SGitHubToolsPRInfos::GetMessageDisplayVisibility ) ];

    contents->AddSlot()
        .Padding( FMargin( 5, 0 ) )
        .FillHeight( 1.0f )
            [ SNew( SBorder )
                    [ SNew( SHorizontalBox ) +
                        SHorizontalBox::Slot()
                            .FillWidth( 0.5f )
                                [ SNew( SVerticalBox ) +
                                    SVerticalBox::Slot()
                                        .AutoHeight()
                                        .Padding( FMargin( 10.0f ) )
                                        .HAlign( HAlign_Fill )
                                            [ SNew( SGitHubToolsPRInfosTreeFilters )
                                                    .TreeViewFilters( TreeViewFilters )
                                                    .OnFiltersChanged( this, &SGitHubToolsPRInfos::OnTreeViewFiltersChanged ) ] +
                                    SVerticalBox::Slot()
                                        .AutoHeight()
                                        .Padding( FMargin( 10.0f ) )
                                        .HAlign( HAlign_Left )
                                            [ SNew( SHorizontalBox ) +
                                                SHorizontalBox::Slot()
                                                    [ SNew( SButton )
                                                            .Text( LOCTEXT( "ExpandAll", "Expand All" ) )
                                                            .OnClicked_Lambda( [ & ]() {
                                                                ExpandAllTreeItems();
                                                                return FReply::Handled();
                                                            } ) ] +
                                                SHorizontalBox::Slot()
                                                    [ SNew( SButton )
                                                            .Text( LOCTEXT( "CollaspeAll", "Collapse All" ) )
                                                            .OnClicked_Lambda( [ & ]() {
                                                                CollapseAllTreeItems();
                                                                return FReply::Handled();
                                                            } ) ] ] +
                                    SVerticalBox::Slot()
                                        .FillHeight( 1.0f )
                                            [ SAssignNew( TreeView, STreeView< FGitHubToolsFileInfosTreeItemPtr > )
                                                    .ItemHeight( 20 )
                                                    .TreeItemsSource( &TreeItems )
                                                    .OnGetChildren( this, &SGitHubToolsPRInfos::OnGetChildrenForTreeView )
                                                    .OnGenerateRow( this, &SGitHubToolsPRInfos::OnGenerateRowForList )
                                                    .OnMouseButtonClick( this, &SGitHubToolsPRInfos::OnSelectedFileChanged )
                                                    .OnMouseButtonDoubleClick( this, &SGitHubToolsPRInfos::OnDiffAgainstRemoteStatusBranchSelected )
                                                    .SelectionMode( ESelectionMode::Single ) ] ] +
                        SHorizontalBox::Slot()
                            .FillWidth( 0.5f )
                                [ SAssignNew( ReviewList, SGitHubToolsPRReviewList )
                                        .PRInfos( PRInfos )
                                        .Visibility( this, &SGitHubToolsPRInfos::GetPRReviewListVisibility )
                                        .OnShouldRebuildFileTreeView( this, &SGitHubToolsPRInfos::OnShouldRebuildTree ) ] ] ];

    contents->AddSlot()
        .AutoHeight()
        .Padding( FMargin( 5, 5, 5, 0 ) )
            [ SNew( SBorder )
                    .Visibility( this, &SGitHubToolsPRInfos::IsWarningPanelVisible )
                    .Padding( 5 )
                        [ SNew( SErrorText )
                                .ErrorText( NSLOCTEXT( "GitHubTools.ReviewWindow", "EmptyToken", "You must define the GitHub Token to be able to see and add comments on assets" ) ) ] ];

    ReviewList->SetEnabled( false );

    ExpandAllTreeItems();
}

void SGitHubToolsPRInfos::ConstructFileInfos()
{
    TreeItems.Reset();
    TreeItems.Reserve( PRInfos->FileInfos.Num() );

    const auto get_path_parts = []( const FString & path ) {
        TArray< FString > parts;
        path.ParseIntoArray( parts, TEXT( "/" ) );

        return parts;
    };

    const auto add_path_to_tree = [ & ]( FGitHubToolsFileInfosTreeItemPtr root_node, TTuple< FGithubToolsPullRequestFileInfosPtr, TArray< FString > > file ) {
        const auto & parts = file.Get< 1 >();
        const auto & file_infos = file.Get< 0 >();

        auto current_node = root_node;

        for ( auto part_index = 1; part_index < parts.Num(); ++part_index )
        {
            bool found = false;
            const auto part = parts[ part_index ];

            for ( auto child : current_node->Children )
            {
                if ( child->Path == part )
                {
                    current_node = child;
                    found = true;
                    break;
                }
            }

            if ( !found )
            {
                if ( GetItemRowVisibility( file_infos ) != EVisibility::Visible )
                {
                    continue;
                }

                auto new_node = MakeShared< FGitHubToolsFileInfosTreeItem >( part );
                current_node->Children.Add( new_node );
                current_node = new_node;

                if ( part_index == parts.Num() - 1 )
                {
                    current_node->FileInfos = file_infos;
                }
            }
        }
    };

    for ( auto file_infos : PRInfos->FileInfos )
    {
        const auto path = file_infos->Path;
        const auto parts = get_path_parts( path );
        const auto root_part = parts[ 0 ];

        FGitHubToolsFileInfosTreeItemPtr node;
        if ( auto * found_node = TreeItems.FindByPredicate( [ parts ]( const auto & tree_item ) {
                 return tree_item->Path == parts[ 0 ];
             } ) )
        {
            node = *found_node;
        }
        else
        {
            node = MakeShared< FGitHubToolsFileInfosTreeItem >( root_part );
            TreeItems.Add( node );
        }

        add_path_to_tree( node, MakeTuple( file_infos, parts ) );

        if ( node->Children.IsEmpty() )
        {
            node->FileInfos = file_infos;
        }
    }
}

void SGitHubToolsPRInfos::OnDiffAgainstRemoteStatusBranchSelected( FGitHubToolsFileInfosTreeItemPtr selected_item )
{
    if ( selected_item->FileInfos == nullptr )
    {
        return;
    }

    if ( !selected_item->FileInfos->IsUAsset() )
    {
        std::string str( StringCast< ANSICHAR >( *selected_item->FileInfos->Path ).Get() );
        const auto hash = picosha2::hash256_hex_string( str );

        TStringBuilder< 512 > url;
        url << PRInfos->URL;
        url << TEXT( "/files#diff-" );
        url << hash.data();

        FPlatformProcess::LaunchURL( *url, nullptr, nullptr );

        return;
    }

    auto * settings = GetDefault< UGitHubToolsSettings >();

    const auto action = [ & ]( FGitHubToolsFileInfosTreeItemPtr item ) {
        if ( item->FileInfos->ChangedState == EGitHubToolsFileChangedState::Added )
        {
            OpenTreeItemAsset( item );
        }

        if ( item->FileInfos->ChangedState == EGitHubToolsFileChangedState::Modified )
        {
            GitHubToolsUtils::DiffFileAgainstOriginStatusBranch( *item->FileInfos );
        }

        OnShouldRebuildTree();
    };

    if ( settings->bMarkFileViewedAutomatically && selected_item->FileInfos->ViewedState != EGitHubToolsFileViewedState::Viewed )
    {
        MarkFileAsViewedAndExecuteCallback( PRInfos->Id, selected_item, action );
    }
    else
    {
        action( selected_item );
    }
}

void SGitHubToolsPRInfos::SetItemExpansion( FGitHubToolsFileInfosTreeItemPtr tree_item, bool is_expanded )
{
    TreeView->SetItemExpansion( tree_item, is_expanded );

    for ( auto child : tree_item->Children )
    {
        SetItemExpansion( child, is_expanded );
    }
}

EVisibility SGitHubToolsPRInfos::GetPRReviewListVisibility() const
{
    return !PRInfos->HasPendingReviews() ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility SGitHubToolsPRInfos::GetMessageDisplayVisibility() const
{
    if ( PRInfos->State != EGitHubToolsPullRequestsState::Open )
    {
        return EVisibility::Visible;
    }
    if ( PRInfos->HasPendingReviews() )
    {
        return EVisibility::Visible;
    }

    return EVisibility::Collapsed;
}

void SGitHubToolsPRInfos::OnShouldRebuildTree() const
{
    TreeView->RebuildList();
}

void SGitHubToolsPRInfos::OnTreeItemStateChanged( TSharedPtr< FGitHubToolsFileInfosTreeItem > tree_item )
{
    OnShouldRebuildTree();
}

void SGitHubToolsPRInfos::OnTreeViewFiltersChanged()
{
    ConstructFileInfos();
    ExpandAllTreeItems();
    TreeView->RequestListRefresh();
}

void SGitHubToolsPRInfos::ExpandAllTreeItems()
{
    for ( auto node : TreeItems )
    {
        SetItemExpansion( node, true );
    }
}

void SGitHubToolsPRInfos::CollapseAllTreeItems()
{
    for ( auto node : TreeItems )
    {
        SetItemExpansion( node, false );
    }
}

bool SGitHubToolsPRInfos::IsFileCommentsButtonEnabled() const
{
    if ( auto * settings = GetDefault< UGitHubToolsSettings >() )
    {
        return !settings->Token.IsEmpty();
    }

    return false;
}

EVisibility SGitHubToolsPRInfos::IsWarningPanelVisible() const
{
    if ( auto * settings = GetDefault< UGitHubToolsSettings >() )
    {
        return settings->Token.IsEmpty()
                   ? EVisibility::Visible
                   : EVisibility::Collapsed;
    }

    return EVisibility::Visible;
}

void SGitHubToolsPRInfos::OnGetChildrenForTreeView( FGitHubToolsFileInfosTreeItemPtr tree_item, TArray< FGitHubToolsFileInfosTreeItemPtr > & children )
{
    children.Append( tree_item->Children );
}

TSharedRef< ITableRow > SGitHubToolsPRInfos::OnGenerateRowForList( FGitHubToolsFileInfosTreeItemPtr tree_item, const TSharedRef< STableViewBase > & owner_table )
{
    return SNew( SGitHubToolsFileInfosRow, owner_table )
        .TreeItem( tree_item )
        .PRId( PRInfos->Id )
        .OnTreeItemStateChanged( this, &SGitHubToolsPRInfos::OnTreeItemStateChanged );
}

EVisibility SGitHubToolsPRInfos::GetItemRowVisibility( FGithubToolsPullRequestFileInfosPtr file_infos ) const
{
    if ( file_infos == nullptr )
    {
        return EVisibility::Visible;
    }

    if ( TreeViewFilters->bShowOnlyUAssets )
    {
        if ( !file_infos->Path.EndsWith( TEXT( ".uasset" ) ) )
        {
            return EVisibility::Collapsed;
        }
    }

    if ( TreeViewFilters->bHideOFPA )
    {
        if ( file_infos->Path.Contains( TEXT( "__ExternalActors__" ) ) ||
             file_infos->Path.Contains( TEXT( "__ExternalObjects__" ) ) )
        {
            return EVisibility::Collapsed;
        }
    }

    if ( TreeViewFilters->bShowOnlyModified )
    {
        if ( file_infos->ChangedState != EGitHubToolsFileChangedState::Modified )
        {
            return EVisibility::Collapsed;
        }
    }

    if ( TreeViewFilters->bShowOnlyUnViewed )
    {
        if ( file_infos->ViewedState != EGitHubToolsFileViewedState::UnViewed )
        {
            return EVisibility::Collapsed;
        }
    }

    if ( TreeViewFilters->bShowOnlyWithoutResolution )
    {
        auto * review = PRInfos->Reviews.FindByPredicate( [ & ]( const FGithubToolsPullRequestReviewThreadInfosPtr & review_infos ) {
            return review_infos->FileName == file_infos->Path;
        } );

        if ( review == nullptr || ( *review )->bIsResolved )
        {
            return EVisibility::Collapsed;
        }
    }

    return EVisibility::Visible;
}

void SGitHubToolsPRInfos::OnSelectedFileChanged( FGitHubToolsFileInfosTreeItemPtr selected_item )
{
    if ( TreeView->GetNumItemsSelected() == 1 )
    {
        ReviewList->ShowFileReviews( selected_item->FileInfos );
    }
    else
    {
        ReviewList->ShowFileReviews( nullptr );
    }
}

void SGitHubToolsFileInfosRow::Construct( const FArguments & arguments, const TSharedRef< STableViewBase > & owner_table_view )
{
    TreeItem = arguments._TreeItem;
    PRId = arguments._PRId;
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
                                                    .Text( LOCTEXT( "MarkAsViewed", "V" ) )
                                                    .IsEnabled( TreeItem->FileInfos->ViewedState != EGitHubToolsFileViewedState::Viewed )
                                                    .OnClicked( this, &SGitHubToolsFileInfosRow::OnMarkAsViewedButtonClicked ) ] +
                                    SHorizontalBox::Slot()
                                        .AutoWidth()
                                            [ SNew( SButton )
                                                    .Text( LOCTEXT( "Open", "O" ) )
                                                    .IsEnabled( TreeItem->FileInfos->ChangedState != EGitHubToolsFileChangedState::Removed )
                                                    .OnClicked( this, &SGitHubToolsFileInfosRow::OnOpenAssetButtonClicked ) ] ] ],
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
        MarkFileAsViewedAndExecuteCallback( PRId, TreeItem, [ &, callback = OnTreeItemStateChanged ]( FGitHubToolsFileInfosTreeItemPtr /*tree_item*/ ) {
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
            OpenTreeItemAsset( item );
        };

        if ( settings->bMarkFileViewedAutomatically && TreeItem->FileInfos->ViewedState != EGitHubToolsFileViewedState::Viewed )
        {
            MarkFileAsViewedAndExecuteCallback( PRId, TreeItem, action );
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

#undef LOCTEXT_NAMESPACE

#endif // SOURCE_CONTROL_WITH_SLATE
