#include "SGitHubToolsPRFilesChanged.h"

#include "SGitHubToolsFileInfosRow.h"
#include "SGitHubToolsMultipleAssetActions.h"
#include "SGitHubToolsPRInfosTreeFilters.h"

#define LOCTEXT_NAMESPACE "GitHubToolsPullRequestReviewWidget"

namespace
{
    class FTreeItemFilterExpressionContext : public ITextFilterExpressionContext
    {
    public:
        explicit FTreeItemFilterExpressionContext( const FGithubToolsPullRequestFileInfosPtr & InSetting ) :
            FileInfos( InSetting )
        {}

        bool TestBasicStringExpression( const FTextFilterString & InValue, const ETextFilterTextComparisonMode InTextComparisonMode ) const override
        {
            return TextFilterUtils::TestBasicStringExpression( FileInfos->Path, InValue, InTextComparisonMode );
        }

        bool TestComplexExpression( const FName & InKey, const FTextFilterString & InValue, const ETextFilterComparisonOperation InComparisonOperation, const ETextFilterTextComparisonMode InTextComparisonMode ) const override
        {
            return false;
        }

    private:
        FGithubToolsPullRequestFileInfosPtr FileInfos;
    };
}

SGitHubToolsPRFilesChanged::~SGitHubToolsPRFilesChanged()
{
}

void SGitHubToolsPRFilesChanged::Construct( const FArguments & arguments )
{
    PRInfos = arguments._PRInfos.Get();
    TreeViewFilters = MakeShared< FGitHubToolsTreeViewFilters >();
    ConstructFileInfos();

    TreeView = SNew( STreeView< FGitHubToolsFileInfosTreeItemPtr > )
                   .TreeItemsSource( &TreeItems )
                   .OnGetChildren( this, &SGitHubToolsPRFilesChanged::OnGetChildrenForTreeView )
                   .OnGenerateRow( this, &SGitHubToolsPRFilesChanged::OnGenerateRowForList )
                   .OnSelectionChanged( this, &SGitHubToolsPRFilesChanged::OnSelectionChanged )
                   .SelectionMode( ESelectionMode::Multi );

    ChildSlot
        [ SNew( SBorder )
                [ SNew( SSplitter )
                        .Orientation( Orient_Horizontal ) +
                    SSplitter::Slot()
                        [ SNew( SVerticalBox ) +
                            SVerticalBox::Slot()
                                .AutoHeight()
                                .Padding( FMargin( 10.0f ) )
                                .HAlign( HAlign_Fill )
                                    [ SNew( SGitHubToolsPRInfosTreeFilters )
                                            .TreeViewFilters( TreeViewFilters )
                                            .OnFiltersChanged( this, &SGitHubToolsPRFilesChanged::OnTreeViewFiltersChanged ) ] +
                            SVerticalBox::Slot()
                                .AutoHeight()
                                .Padding( FMargin( 10.0f ) )
                                    [ SNew( SHorizontalBox ) +
                                        SHorizontalBox::Slot()
                                            .AutoWidth()
                                                [ SNew( SButton )
                                                        .Text( LOCTEXT( "ExpandAll", "Expand All" ) )
                                                        .HAlign( HAlign_Center )
                                                        .OnClicked_Lambda( [ & ]() {
                                                            ExpandAllTreeItems();
                                                            return FReply::Handled();
                                                        } ) ] +
                                        SHorizontalBox::Slot()
                                            .AutoWidth()
                                                [ SNew( SButton )
                                                        .Text( LOCTEXT( "CollapseAll", "Collapse All" ) )
                                                        .HAlign( HAlign_Center )
                                                        .OnClicked_Lambda( [ & ]() {
                                                            CollapseAllTreeItems();
                                                            return FReply::Handled();
                                                        } ) ] +
                                        SHorizontalBox::Slot()
                                            .FillWidth( 1.0f )
                                            .HAlign( HAlign_Right )
                                                [ SNew( SGitHubToolsMultipleAssetActions )
                                                        .TreeView( TreeView )
                                                        .PRInfos( PRInfos )
                                                        .OnFileInfosStateChanged( this, &SGitHubToolsPRFilesChanged::OnMultipleFileInfosStateChanged ) ] ] +
                            SVerticalBox::Slot()
                                .FillHeight( 1.0f )
                                    [ TreeView->AsShared() ] ] +
                    SSplitter::Slot()
                        [ SAssignNew( ReviewList, SGitHubToolsPRReviewList )
                                .PRInfos( PRInfos )
                                .Visibility( this, &SGitHubToolsPRFilesChanged::GetPRReviewListVisibility )
                                .OnShouldRebuildFileTreeView( this, &SGitHubToolsPRFilesChanged::OnShouldRebuildTree ) ] ] ];

    ExpandAllTreeItems();
    ReviewList->SetEnabled( false );
}

int SGitHubToolsPRFilesChanged::GetSelectedFilesCount() const
{
    return TreeView->GetNumItemsSelected();
}

void SGitHubToolsPRFilesChanged::ConstructFileInfos()
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

void SGitHubToolsPRFilesChanged::OnGetChildrenForTreeView( FGitHubToolsFileInfosTreeItemPtr tree_item, TArray< FGitHubToolsFileInfosTreeItemPtr > & children )
{
    children.Append( tree_item->Children );
}

TSharedRef< ITableRow > SGitHubToolsPRFilesChanged::OnGenerateRowForList( FGitHubToolsFileInfosTreeItemPtr tree_item, const TSharedRef< STableViewBase > & owner_table )
{
    return SNew( SGitHubToolsFileInfosRow, owner_table )
        .TreeItem( tree_item )
        .PRInfos( PRInfos )
        .OnFileInfosStateChanged( this, &SGitHubToolsPRFilesChanged::OnFileInfosStateChanged );
}

void SGitHubToolsPRFilesChanged::OnSelectionChanged( TSharedPtr< FGitHubToolsFileInfosTreeItem > selected_item, ESelectInfo::Type Arg )
{
    if ( TreeView->GetNumItemsSelected() == 1 )
    {
        ReviewList->ShowFileReviews( selected_item->FileInfos );

        if ( selected_item->FileInfos == nullptr )
        {
            TArray< FGitHubToolsFileInfosTreeItemPtr > children_items;
            for ( auto child : selected_item->Children )
            {
                children_items.Add( child );
                RecursivelySelectChildren( children_items, child );
            }

            for ( auto child : children_items )
            {
                TreeView->Private_SetItemSelection( child, true, false );
            }
        }
    }
    else
    {
        ReviewList->ShowFileReviews( nullptr );
    }
}

EVisibility SGitHubToolsPRFilesChanged::GetItemRowVisibility( FGithubToolsPullRequestFileInfosPtr file_infos ) const
{
    if ( file_infos == nullptr )
    {
        return EVisibility::Visible;
    }

    if ( TreeViewFilters->bShowOnlyUAssets )
    {
        if ( !file_infos->IsUAsset() )
        {
            return EVisibility::Collapsed;
        }
    }

    if ( TreeViewFilters->bHideDeveloperFolder )
    {
        if ( file_infos->IsFromDeveloperFolder() )
        {
            return EVisibility::Collapsed;
        }
    }

    if ( TreeViewFilters->bHideOFPA )
    {
        if ( file_infos->IsOFPAAsset() )
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
        if ( file_infos->ViewedState == EGitHubToolsFileViewedState::Viewed )
        {
            return EVisibility::Collapsed;
        }
    }

    if ( TreeViewFilters->bShowOnlyDismissed )
    {
        if ( file_infos->ViewedState != EGitHubToolsFileViewedState::Dismissed )
        {
            return EVisibility::Collapsed;
        }
    }

    if ( TreeViewFilters->bShowOnlyWithoutResolution )
    {
        if ( !file_infos->bHasUnresolvedConversations )
        {
            return EVisibility::Collapsed;
        }
    }

    if ( !TreeViewFilters->SearchTextEvaluator.TestTextFilter( FTreeItemFilterExpressionContext( file_infos ) ) )
    {
        return EVisibility::Collapsed;
    }

    return EVisibility::Visible;
}

void SGitHubToolsPRFilesChanged::RecursivelySelectChildren( TArray< FGitHubToolsFileInfosTreeItemPtr > & children, FGitHubToolsFileInfosTreeItemPtr item )
{
    for ( auto child : item->Children )
    {
        children.Add( child );
        RecursivelySelectChildren( children, child );
    }
}

void SGitHubToolsPRFilesChanged::OnTreeViewFiltersChanged()
{
    ConstructFileInfos();
    TreeView->RequestListRefresh();
    ExpandAllTreeItems();
}

void SGitHubToolsPRFilesChanged::ExpandAllTreeItems()
{
    for ( auto node : TreeItems )
    {
        SetItemExpansion( node, true );
    }
}

void SGitHubToolsPRFilesChanged::CollapseAllTreeItems()
{
    for ( auto node : TreeItems )
    {
        SetItemExpansion( node, false );
    }
}

void SGitHubToolsPRFilesChanged::SetItemExpansion( FGitHubToolsFileInfosTreeItemPtr tree_item, bool is_expanded )
{
    TreeView->SetItemExpansion( tree_item, is_expanded );

    for ( auto child : tree_item->Children )
    {
        SetItemExpansion( child, is_expanded );
    }
}

void SGitHubToolsPRFilesChanged::OnFileInfosStateChanged( FGithubToolsPullRequestFileInfosPtr file_infos )
{
    OnShouldRebuildTree();
}

void SGitHubToolsPRFilesChanged::OnMultipleFileInfosStateChanged( const TArray< FGithubToolsPullRequestFileInfosPtr > & file_infos )
{
    OnTreeViewFiltersChanged();
}

void SGitHubToolsPRFilesChanged::OnShouldRebuildTree() const
{
    TreeView->RebuildList();
    TreeView->RequestListRefresh();
}

EVisibility SGitHubToolsPRFilesChanged::GetPRReviewListVisibility() const
{
    return /* !PRInfos->HasPendingReviews() ? */ EVisibility::Visible /* : EVisibility::Collapsed */;
}

#undef LOCTEXT_NAMESPACE