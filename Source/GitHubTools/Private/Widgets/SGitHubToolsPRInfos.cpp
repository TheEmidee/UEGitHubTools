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
    const FName ColumnID_CheckBoxLabel( "CheckBox" );
    const FName ColumnID_FileChangedStateIconLabel( "FileChangedStateIcon" );
    const FName ColumnID_FileViewedStateIconLabel( "FileViewedStateIcon" );
    const FName ColumnID_FileLabel( "File" );

    const float CheckBoxColumnWidth = 23.0f;
    const float IconColumnWidth = 21.0f;
    const float CommentButtonColumnWidth = 120.0f;
}

SGitHubToolsPRInfos::~SGitHubToolsPRInfos()
{
}

void SGitHubToolsPRInfos::Construct( const FArguments & arguments )
{
    ParentFrame = arguments._ParentWindow.Get();
    SortByColumn = SGitSourceControlReviewFilesWidgetDefs::ColumnID_FileLabel;
    SortMode = EColumnSortMode::Ascending;

    PRInfos = arguments._Infos.Get();

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
                                            .Text( LOCTEXT( "OpenInGitHub", "Open in GitHub" ) )
                                            .OnClicked( this, &SGitHubToolsPRInfos::OpenInGitHubClicked ) ] +
                            SHorizontalBox::Slot()
                                .FillWidth( 1.0f )
                                .Padding( FMargin( 10 ) )
                                    [ SNew( STextBlock )
                                            .Text( PRInfos->Title )
                                            .Justification( ETextJustify::Type::Left ) ] ] ];

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

    const TSharedRef< SHeaderRow > header_row_widget = SNew( SHeaderRow );

    header_row_widget->AddColumn(
        SHeaderRow::Column( SGitSourceControlReviewFilesWidgetDefs::ColumnID_CheckBoxLabel )
            [ SNew( SCheckBox ) ]
                .FixedWidth( SGitSourceControlReviewFilesWidgetDefs::CheckBoxColumnWidth ) );

    header_row_widget->AddColumn(
        SHeaderRow::Column( SGitSourceControlReviewFilesWidgetDefs::ColumnID_FileChangedStateIconLabel )
            [ SNew( SSpacer ) ]
                .SortMode( this, &SGitHubToolsPRInfos::GetColumnSortMode, SGitSourceControlReviewFilesWidgetDefs::ColumnID_FileChangedStateIconLabel )
                .OnSort( this, &SGitHubToolsPRInfos::OnColumnSortModeChanged )
                .FixedWidth( SGitSourceControlReviewFilesWidgetDefs::IconColumnWidth ) );

    header_row_widget->AddColumn(
        SHeaderRow::Column( SGitSourceControlReviewFilesWidgetDefs::ColumnID_FileViewedStateIconLabel )
            [ SNew( SSpacer ) ]
                .SortMode( this, &SGitHubToolsPRInfos::GetColumnSortMode, SGitSourceControlReviewFilesWidgetDefs::ColumnID_FileViewedStateIconLabel )
                .OnSort( this, &SGitHubToolsPRInfos::OnColumnSortModeChanged )
                .FixedWidth( SGitSourceControlReviewFilesWidgetDefs::IconColumnWidth ) );

    header_row_widget->AddColumn(
        SHeaderRow::Column( SGitSourceControlReviewFilesWidgetDefs::ColumnID_FileLabel )
            .DefaultLabel( LOCTEXT( "FileColumnLabel", "File" ) )
            .SortMode( this, &SGitHubToolsPRInfos::GetColumnSortMode, SGitSourceControlReviewFilesWidgetDefs::ColumnID_FileLabel )
            .OnSort( this, &SGitHubToolsPRInfos::OnColumnSortModeChanged )
            .FillWidth( 7.0f ) );

    contents->AddSlot()
        .Padding( FMargin( 5, 0 ) )
        .FillHeight( 1.0f )
            [ SNew( SBorder )
                    [ SNew( SHorizontalBox ) +
                        SHorizontalBox::Slot()
                            .FillWidth( 0.5f )
                                [ SAssignNew( ListView, SListView< FGithubToolsPullRequestFileInfosPtr > )
                                        .ItemHeight( 20 )
                                        .ListItemsSource( &PRInfos->FileInfos )
                                        .OnGenerateRow( this, &SGitHubToolsPRInfos::OnGenerateRowForList )
                                        .OnMouseButtonClick( this, &SGitHubToolsPRInfos::OnSelectedFileChanged )
                                        .OnMouseButtonDoubleClick( this, &SGitHubToolsPRInfos::OnDiffAgainstRemoteStatusBranchSelected )
                                        .HeaderRow( header_row_widget )
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

void SGitHubToolsPRInfos::OnDiffAgainstRemoteStatusBranchSelected( FGithubToolsPullRequestFileInfosPtr selected_item )
{
    if ( selected_item->ChangedState == EGitHubToolsFileChangedState::Added )
    {
        const auto asset_data = GitHubToolsGitUtils::GetAssetDataFromFileInfos( *selected_item );
        if ( asset_data.IsSet() )
        {
            const auto & asset_tools_module = FModuleManager::GetModuleChecked< FAssetToolsModule >( "AssetTools" );
            asset_tools_module.Get().OpenEditorForAssets( { asset_data.GetValue().GetAsset() } );
            //selected_item->SetCheckBoxState( ECheckBoxState::Checked );
            return;
        }
    }

    if ( selected_item->ChangedState == EGitHubToolsFileChangedState::Modified )
    {
        GitHubToolsGitUtils::DiffFileAgainstOriginStatusBranch( *selected_item );
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

TSharedRef< ITableRow > SGitHubToolsPRInfos::OnGenerateRowForList( FGithubToolsPullRequestFileInfosPtr file_infos, const TSharedRef< STableViewBase > & owner_table )
{
    return SNew( SGitHubToolsFileInfosRow, owner_table )
        .FileInfos( file_infos )
        .Visibility( MakeAttributeLambda( [ &, file_infos ]() {
            return GetItemRowVisibility( file_infos );
        } ) );
}

EVisibility SGitHubToolsPRInfos::GetItemRowVisibility( FGithubToolsPullRequestFileInfosPtr file_infos ) const
{
    if ( OnlyShowAssetsCheckBox->GetCheckedState() == ECheckBoxState::Checked )
    {
        if ( !file_infos->FileName.EndsWith( TEXT( ".uasset" ) ) )
        {
            return EVisibility::Collapsed;
        }
    }

    if ( HideOFPACheckBox->GetCheckedState() == ECheckBoxState::Checked )
    {
        if ( file_infos->FileName.Contains( TEXT( "__ExternalActors__" ) ) ||
             file_infos->FileName.Contains( TEXT( "__ExternalObjects__" ) ) )
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

    ListView->RequestListRefresh();
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

void SGitHubToolsPRInfos::OnSelectedFileChanged( FGithubToolsPullRequestFileInfosPtr selected_item )
{
    ReviewList->SetEnabled( true );
    ReviewList->ShowFileReviews( selected_item );
}

void SGitHubToolsFileInfosRow::Construct( const FArguments & arguments, const TSharedRef< STableViewBase > & owner_table_view )
{
    FileInfos = arguments._FileInfos;

    SMultiColumnTableRow< FGithubToolsPullRequestFileInfosPtr >::Construct( FSuperRowType::FArguments(), owner_table_view );
}

TSharedRef< SWidget > SGitHubToolsFileInfosRow::GenerateWidgetForColumn( const FName & column_name )
{
    const FMargin row_padding( 3, 0, 0, 0 );

    TSharedPtr< SWidget > item_content_widget;

    if ( column_name == SGitSourceControlReviewFilesWidgetDefs::ColumnID_CheckBoxLabel )
    {
        item_content_widget = SNew( SHorizontalBox ) +
                              SHorizontalBox::Slot()
                                  .Padding( row_padding )
                                      [ SNew( SCheckBox )
                                          /*.IsChecked( item.Get(), &FGithubToolsPullRequestFileInfos::GetCheckBoxState )
                                                                       .OnCheckStateChanged( item.Get(), &FGithubToolsPullRequestFileInfos::SetCheckBoxState ) */
        ];
    }
    else if ( column_name == SGitSourceControlReviewFilesWidgetDefs::ColumnID_FileChangedStateIconLabel )
    {
        item_content_widget = SNew( SHorizontalBox ) +
                              SHorizontalBox::Slot()
                                  .HAlign( HAlign_Center )
                                  .VAlign( VAlign_Center )
                                      [ SNew( SImage )
                                              .Image( FRevisionControlStyleManager::Get().GetBrush( FileInfos->ChangedStateIconName ) )
                                              .ToolTipText( FileInfos->ChangedStateToolTip ) ];
    }
    else if ( column_name == SGitSourceControlReviewFilesWidgetDefs::ColumnID_FileViewedStateIconLabel )
    {
        item_content_widget = SNew( SHorizontalBox ) +
                              SHorizontalBox::Slot()
                                  .HAlign( HAlign_Center )
                                  .VAlign( VAlign_Center )
                                      [ SNew( SImage )
                                              .Image( FRevisionControlStyleManager::Get().GetBrush( FileInfos->ViewedStateIconName ) )
                                              .ToolTipText( FileInfos->ViewedStateToolTip ) ];
    }
    else if ( column_name == SGitSourceControlReviewFilesWidgetDefs::ColumnID_FileLabel )
    {
        item_content_widget = SNew( SHorizontalBox ) +
                              SHorizontalBox::Slot()
                                  .Padding( row_padding )
                                      [ SNew( STextBlock )
                                              .Text( FileInfos->PackageName ) ];
    }

    return item_content_widget.ToSharedRef();
}

#undef LOCTEXT_NAMESPACE

#endif // SOURCE_CONTROL_WITH_SLATE
