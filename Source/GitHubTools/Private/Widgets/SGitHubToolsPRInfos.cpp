#include "SGitHubToolsPRInfos.h"

#include "GitHubTools.h"
#include "GitHubToolsGitUtils.h"
#include "GitHubToolsSettings.h"
#include "MaterialGraph/MaterialGraphSchema.h"
#include "SGitHubToolsPRReviewList.h"

#include <AssetToolsModule.h>
#include <RevisionControlStyle/RevisionControlStyle.h>

#if SOURCE_CONTROL_WITH_SLATE

#define LOCTEXT_NAMESPACE "GitHubToolsPullRequestReviewWidget"

namespace SGitSourceControlReviewFilesWidgetDefs
{
    const FName ColumnID_FileChangedStateIconLabel( "FileChangedStateIcon" );
    const FName ColumnID_FileViewedStateIconLabel( "FileViewedStateIcon" );
    const FName ColumnID_FileLabel( "File" );

    const float IconColumnWidth = 21.0f;
    const float CommentButtonColumnWidth = 120.0f;
}

SGitHubToolsPRInfos::~SGitHubToolsPRInfos()
{
}

void SGitHubToolsPRInfos::Construct( const FArguments & arguments )
{
    SortByColumn = SGitSourceControlReviewFilesWidgetDefs::ColumnID_FileLabel;
    SortMode = EColumnSortMode::Ascending;

    PRInfos = arguments._Infos.Get();

    ConstructFileInfos();

    TSharedPtr< SVerticalBox > contents;

    ChildSlot
        [ SNew( SBorder )
                .BorderImage( FAppStyle::GetBrush( "ToolPanel.GroupBorder" ) )
                    [ SAssignNew( contents, SVerticalBox ) ] ];

    contents->AddSlot()
        .Padding( FMargin( 5 ) )
        .AutoHeight()
            [ SNew( SBorder )
                    .Padding( FMargin( 10 ) )
                        [ SNew( SHorizontalBox ) +
                            SHorizontalBox::Slot()
                                .AutoWidth()
                                .Padding( FMargin( 10 ) )
                                    [ SNew( SButton )
                                            .VAlign( VAlign_Center )
                                            .Text( LOCTEXT( "OpenInGitHub", "Open in GitHub" ) )
                                            .OnClicked( this, &SGitHubToolsPRInfos::OpenInGitHubClicked ) ] +
                            SHorizontalBox::Slot()
                                .FillWidth( 1.0f )
                                .Padding( FMargin( 10 ) )
                                    [ SNew( SVerticalBox ) +
                                        SVerticalBox::Slot()
                                            .AutoHeight()
                                                [ SNew( STextBlock )
                                                        .Text( PRInfos->Author )
                                                        .Justification( ETextJustify::Type::Left ) ] +
                                        SVerticalBox::Slot()
                                            .AutoHeight()
                                                [ SNew( STextBlock )
                                                        .Text( FText::FromString( FString::Printf( TEXT( "%s ( # %i )" ), *PRInfos->Title, PRInfos->Number ) ) )
                                                        .Justification( ETextJustify::Type::Left ) ] ]

    ] ];

    contents->AddSlot()
        .Padding( FMargin( 5 ) )
        .AutoHeight()
            [ SNew( SBorder )
                    .Padding( FMargin( 10 ) )
                        [ SNew( SHorizontalBox ) +
                            SHorizontalBox::Slot()
                                .AutoWidth()
                                .Padding( FMargin( 5 ) )
                                    [ SAssignNew( OnlyShowAssetsCheckBox, SCheckBox )
                                            .ToolTipText( LOCTEXT( "OnlyShowUAssetsToolTip", "Toggle whether or not to only show uasset files." ) )
                                            .Type( ESlateCheckBoxType::CheckBox )
                                            .IsChecked( ECheckBoxState::Checked )
                                            .OnCheckStateChanged( this, &SGitHubToolsPRInfos::OnShowOnlyUAssetsCheckStateChanged )
                                            .Padding( 4.f )
                                                [ SNew( STextBlock )
                                                        .Text( LOCTEXT( "OnlyShowUAssets", "Only show uassets" ) ) ] ] +
                            SHorizontalBox::Slot()
                                .AutoWidth()
                                .Padding( FMargin( 5 ) )
                                    [ SAssignNew( HideOFPACheckBox, SCheckBox )
                                            .ToolTipText( LOCTEXT( "HideOFPAToolTip", "Hide OFPA assets." ) )
                                            .Type( ESlateCheckBoxType::CheckBox )
                                            .IsChecked( ECheckBoxState::Checked )
                                            .OnCheckStateChanged( this, &SGitHubToolsPRInfos::OnHideOFPACheckStateChanged )
                                            .Padding( 4.f )
                                                [ SNew( STextBlock )
                                                        .Text( LOCTEXT( "HideOFPA", "Hide OFPA assets" ) ) ] ]

    ] ];

    contents->AddSlot()
        .Padding( FMargin( 5, 0 ) )
        .FillHeight( 1.0f )
            [ SNew( SBorder )
                    [ SNew( SHorizontalBox ) +
                        SHorizontalBox::Slot()
                            .FillWidth( 0.5f )
                                [ SAssignNew( TreeView, STreeView< FGitHubToolsFileInfosTreeItemPtr > )
                                        .ItemHeight( 20 )
                                        .TreeItemsSource( &TreeItems )
                                        .OnGetChildren( this, &SGitHubToolsPRInfos::OnGetChildrenForTreeView )
                                        .OnGenerateRow( this, &SGitHubToolsPRInfos::OnGenerateRowForList )
                                        .OnMouseButtonClick( this, &SGitHubToolsPRInfos::OnSelectedFileChanged )
                                        .OnMouseButtonDoubleClick( this, &SGitHubToolsPRInfos::OnDiffAgainstRemoteStatusBranchSelected )
                                        .SelectionMode( ESelectionMode::Single ) ] +
                        SHorizontalBox::Slot()
                            .FillWidth( 0.5f )
                                [ SAssignNew( ReviewList, SGitHubToolsPRReviewList )
                                        .PRInfos( PRInfos ) ] ] ];

    contents->AddSlot()
        .AutoHeight()
        .Padding( FMargin( 5, 5, 5, 0 ) )
            [ SNew( SBorder )
                    .Visibility( this, &SGitHubToolsPRInfos::IsWarningPanelVisible )
                    .Padding( 5 )
                        [ SNew( SErrorText )
                                .ErrorText( NSLOCTEXT( "GitHubTools.ReviewWindow", "EmptyToken", "You must define the GitHub Token to be able to see and add comments on assets" ) ) ] ];

    ReviewList->SetEnabled( false );
    RequestSort();
}

void SGitHubToolsPRInfos::ConstructFileInfos()
{
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
    }
}

void SGitHubToolsPRInfos::OnDiffAgainstRemoteStatusBranchSelected( FGitHubToolsFileInfosTreeItemPtr selected_item )
{
    if ( selected_item->FileInfos->ChangedState == EGitHubToolsFileChangedState::Added )
    {
        const auto asset_data = GitHubToolsUtils::GetAssetDataFromFileInfos( *selected_item->FileInfos );
        if ( asset_data.IsSet() )
        {
            const auto & asset_tools_module = FModuleManager::GetModuleChecked< FAssetToolsModule >( "AssetTools" );
            asset_tools_module.Get().OpenEditorForAssets( { asset_data.GetValue().GetAsset() } );
            //selected_item->SetCheckBoxState( ECheckBoxState::Checked );
            return;
        }
    }

    if ( selected_item->FileInfos->ChangedState == EGitHubToolsFileChangedState::Modified )
    {
        GitHubToolsUtils::DiffFileAgainstOriginStatusBranch( *selected_item->FileInfos );
        //selected_item->SetCheckBoxState( ECheckBoxState::Checked );
    }
}

FReply SGitHubToolsPRInfos::OpenInGitHubClicked()
{
    auto * settings = GetDefault< UGitHubToolsSettings >();

    TStringBuilder< 256 > url;
    url << TEXT( "https://github.com/" );
    url << settings->RepositoryOwner;
    url << TEXT( "/" );
    url << settings->RepositoryName;
    url << TEXT( "/pull/" );
    url << PRInfos->Number;

    FPlatformProcess::LaunchURL( *url, nullptr, nullptr );

    return FReply::Handled();
}

void SGitHubToolsPRInfos::OnShowOnlyUAssetsCheckStateChanged( ECheckBoxState new_state )
{
    RequestSort();
}

void SGitHubToolsPRInfos::OnHideOFPACheckStateChanged( ECheckBoxState new_state )
{
    RequestSort();
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
    //return SNew( STableRow< FText >, owner_table )
    //    .Style( FAppStyle::Get(), "GameplayTagTreeView" )
    //        [ SNew( SHorizontalBox )

    //            // Tag Selection (selection mode only)
    //            + SHorizontalBox::Slot()
    //                  .FillWidth( 1.0f )
    //                  .HAlign( HAlign_Left )
    //                  .VAlign( VAlign_Center )
    //                      [ SNew( STextBlock ).Text( FText::FromString( file_infos->Path ) ) ] ];

    return SNew( SGitHubToolsFileInfosRow, owner_table )
        .TreeItem( tree_item )
        .Visibility( MakeAttributeLambda( [ &, tree_item ]() {
            return GetItemRowVisibility( tree_item->FileInfos );
        } ) );
}

EVisibility SGitHubToolsPRInfos::GetItemRowVisibility( FGithubToolsPullRequestFileInfosPtr file_infos ) const
{
    if ( file_infos == nullptr )
    {
        return EVisibility::Visible;
    }

    if ( OnlyShowAssetsCheckBox->GetCheckedState() == ECheckBoxState::Checked )
    {
        if ( !file_infos->Path.EndsWith( TEXT( ".uasset" ) ) )
        {
            return EVisibility::Collapsed;
        }
    }

    if ( HideOFPACheckBox->GetCheckedState() == ECheckBoxState::Checked )
    {
        if ( file_infos->Path.Contains( TEXT( "__ExternalActors__" ) ) ||
             file_infos->Path.Contains( TEXT( "__ExternalObjects__" ) ) )
        {
            return EVisibility::Collapsed;
        }
    }

    return EVisibility::Visible;
}

EColumnSortMode::Type SGitHubToolsPRInfos::GetColumnSortMode( const FName column_id ) const
{
    if ( SortByColumn != column_id )
    {
        return EColumnSortMode::None;
    }

    return SortMode;
}

void SGitHubToolsPRInfos::OnColumnSortModeChanged( const EColumnSortPriority::Type /*sort_priority*/, const FName & column_id, const EColumnSortMode::Type sort_mode )
{
    SortByColumn = column_id;
    SortMode = sort_mode;

    RequestSort();
}

void SGitHubToolsPRInfos::RequestSort()
{
    SortTree();

    TreeView->RequestListRefresh();

    for ( auto root_node : TreeItems )
    {
        TreeView->SetItemExpansion( root_node, true );
    }
}

void SGitHubToolsPRInfos::SortTree()
{
    if ( SortByColumn == SGitSourceControlReviewFilesWidgetDefs::ColumnID_FileLabel )
    {
        if ( SortMode == EColumnSortMode::Ascending )
        {
            PRInfos->FileInfos.Sort( []( const FGithubToolsPullRequestFileInfosPtr & A, const FGithubToolsPullRequestFileInfosPtr & B ) {
                return A->PackageName.ToString() < B->PackageName.ToString();
            } );
        }
        else if ( SortMode == EColumnSortMode::Descending )
        {
            PRInfos->FileInfos.Sort( []( const FGithubToolsPullRequestFileInfosPtr & A, const FGithubToolsPullRequestFileInfosPtr & B ) {
                return A->PackageName.ToString() >= B->PackageName.ToString();
            } );
        }
    }
    else if ( SortByColumn == SGitSourceControlReviewFilesWidgetDefs::ColumnID_FileChangedStateIconLabel )
    {
        if ( SortMode == EColumnSortMode::Ascending )
        {
            PRInfos->FileInfos.Sort( []( const FGithubToolsPullRequestFileInfosPtr & A, const FGithubToolsPullRequestFileInfosPtr & B ) {
                return A->ChangedStateIconName.ToString() < B->ChangedStateIconName.ToString();
            } );
        }
        else if ( SortMode == EColumnSortMode::Descending )
        {
            PRInfos->FileInfos.Sort( []( const FGithubToolsPullRequestFileInfosPtr & A, const FGithubToolsPullRequestFileInfosPtr & B ) {
                return A->ChangedStateIconName.ToString() >= B->ChangedStateIconName.ToString();
            } );
        }
    }
    else if ( SortByColumn == SGitSourceControlReviewFilesWidgetDefs::ColumnID_FileViewedStateIconLabel )
    {
        if ( SortMode == EColumnSortMode::Ascending )
        {
            PRInfos->FileInfos.Sort( []( const FGithubToolsPullRequestFileInfosPtr & A, const FGithubToolsPullRequestFileInfosPtr & B ) {
                return A->ViewedStateIconName.ToString() < B->ViewedStateIconName.ToString();
            } );
        }
        else if ( SortMode == EColumnSortMode::Descending )
        {
            PRInfos->FileInfos.Sort( []( const FGithubToolsPullRequestFileInfosPtr & A, const FGithubToolsPullRequestFileInfosPtr & B ) {
                return A->ViewedStateIconName.ToString() >= B->ViewedStateIconName.ToString();
            } );
        }
    }
}

void SGitHubToolsPRInfos::OnSelectedFileChanged( FGitHubToolsFileInfosTreeItemPtr selected_item )
{
    ReviewList->ShowFileReviews( selected_item->FileInfos );
}

void SGitHubToolsFileInfosRow::Construct( const FArguments & arguments, const TSharedRef< STableViewBase > & owner_table_view )
{
    TreeItem = arguments._TreeItem;

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
                                        .Text( FText::FromString( TreeItem->Path ) ) ] ],
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

#undef LOCTEXT_NAMESPACE

#endif // SOURCE_CONTROL_WITH_SLATE
