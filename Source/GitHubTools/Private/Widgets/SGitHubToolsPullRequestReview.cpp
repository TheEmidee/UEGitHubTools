#include "SGitHubToolsPullRequestReview.h"

#include "GitHubTools.h"
#include "GitHubToolsGitUtils.h"
#include "GitHubToolsSettings.h"
#include "SGitHubToolsFileComments.h"

#include <AssetToolsModule.h>
#include <RevisionControlStyle/RevisionControlStyle.h>

#if SOURCE_CONTROL_WITH_SLATE

#define LOCTEXT_NAMESPACE "GitHubToolsPullRequestReviewWidget"

namespace SGitSourceControlReviewFilesWidgetDefs
{
    const FName ColumnID_CheckBoxLabel( "CheckBox" );
    const FName ColumnID_IconLabel( "Icon" );
    const FName ColumnID_AssetLabel( "Asset" );
    const FName ColumnID_CommentLabel( "Comment" );
    const FName ColumnID_FileLabel( "File" );

    const float CheckBoxColumnWidth = 23.0f;
    const float IconColumnWidth = 21.0f;
    const float CommentButtonColumnWidth = 120.0f;
}

SGitHubToolsPullRequestReview::~SGitHubToolsPullRequestReview()
{
}

void SGitHubToolsPullRequestReview::Construct( const FArguments & arguments )
{
    ParentFrame = arguments._ParentWindow.Get();
    SortByColumn = SGitSourceControlReviewFilesWidgetDefs::ColumnID_AssetLabel;
    SortMode = EColumnSortMode::Ascending;

    ListViewItems = arguments._Files.Get();

    const TSharedRef< SHeaderRow > header_row_widget = SNew( SHeaderRow );

    header_row_widget->AddColumn(
        SHeaderRow::Column( SGitSourceControlReviewFilesWidgetDefs::ColumnID_CheckBoxLabel )
            [ SNew( SCheckBox ) ]
                .FixedWidth( SGitSourceControlReviewFilesWidgetDefs::CheckBoxColumnWidth ) );

    header_row_widget->AddColumn(
        SHeaderRow::Column( SGitSourceControlReviewFilesWidgetDefs::ColumnID_IconLabel )
            [ SNew( SSpacer ) ]
                .SortMode( this, &SGitHubToolsPullRequestReview::GetColumnSortMode, SGitSourceControlReviewFilesWidgetDefs::ColumnID_IconLabel )
                .OnSort( this, &SGitHubToolsPullRequestReview::OnColumnSortModeChanged )
                .FixedWidth( SGitSourceControlReviewFilesWidgetDefs::IconColumnWidth ) );

    header_row_widget->AddColumn(
        SHeaderRow::Column( SGitSourceControlReviewFilesWidgetDefs::ColumnID_CommentLabel )
            .DefaultLabel( LOCTEXT( "AssetColumnLabel", "Comment" ) )
            /*.SortMode( this, &SGitHubToolsPullRequestReviewWidget::GetColumnSortMode, SGitSourceControlReviewFilesWidgetDefs::ColumnID_AssetLabel )
            .OnSort( this, &SGitHubToolsPullRequestReviewWidget::OnColumnSortModeChanged )*/
            .FixedWidth( SGitSourceControlReviewFilesWidgetDefs::CommentButtonColumnWidth ) );

    header_row_widget->AddColumn(
        SHeaderRow::Column( SGitSourceControlReviewFilesWidgetDefs::ColumnID_AssetLabel )
            .DefaultLabel( LOCTEXT( "AssetColumnLabel", "Asset" ) )
            .SortMode( this, &SGitHubToolsPullRequestReview::GetColumnSortMode, SGitSourceControlReviewFilesWidgetDefs::ColumnID_AssetLabel )
            .OnSort( this, &SGitHubToolsPullRequestReview::OnColumnSortModeChanged )
            .FillWidth( 5.0f ) );

    header_row_widget->AddColumn(
        SHeaderRow::Column( SGitSourceControlReviewFilesWidgetDefs::ColumnID_FileLabel )
            .DefaultLabel( LOCTEXT( "FileColumnLabel", "File" ) )
            .SortMode( this, &SGitHubToolsPullRequestReview::GetColumnSortMode, SGitSourceControlReviewFilesWidgetDefs::ColumnID_FileLabel )
            .OnSort( this, &SGitHubToolsPullRequestReview::OnColumnSortModeChanged )
            .FillWidth( 7.0f ) );

    TSharedPtr< SVerticalBox > contents;

    ChildSlot
        [ SNew( SBorder )
                .BorderImage( FAppStyle::GetBrush( "ToolPanel.GroupBorder" ) )
                    [ SAssignNew( contents, SVerticalBox ) ] ];

    contents->AddSlot()
        .Padding( FMargin( 5, 0 ) )
            [ SNew( SBorder )
                    [ SAssignNew( ListView, SListView< FGithubToolsPullRequestFileInfosPtr > )
                            .ItemHeight( 20 )
                            .ListItemsSource( &ListViewItems )
                            .OnGenerateRow( this, &SGitHubToolsPullRequestReview::OnGenerateRowForList )
                            .OnMouseButtonDoubleClick( this, &SGitHubToolsPullRequestReview::OnDiffAgainstRemoteStatusBranchSelected )
                            .HeaderRow( header_row_widget )
                            .SelectionMode( ESelectionMode::None ) ] ];

    contents->AddSlot()
        .AutoHeight()
        .Padding( FMargin( 5, 5, 5, 0 ) )
            [ SNew( SBorder )
                    .Visibility( this, &SGitHubToolsPullRequestReview::IsWarningPanelVisible )
                    .Padding( 5 )
                        [ SNew( SErrorText )
                                .ErrorText( NSLOCTEXT( "GitHubTools.ReviewWindow", "EmptyToken", "You must define the GitHub Token to be able to see and add comments on assets" ) ) ] ];
    RequestSort();
}

void SGitHubToolsPullRequestReview::OnDiffAgainstRemoteStatusBranchSelected( FGithubToolsPullRequestFileInfosPtr selected_item )
{
    if ( selected_item->FileState == EGitHubToolsFileState::Added )
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

    if ( selected_item->FileState == EGitHubToolsFileState::Modified )
    {
        GitHubToolsGitUtils::DiffFileAgainstOriginStatusBranch( *selected_item );
        //selected_item->SetCheckBoxState( ECheckBoxState::Checked );
    }
}

FReply SGitHubToolsPullRequestReview::OnKeyDown( const FGeometry & my_geometry, const FKeyEvent & key_event )
{
    if ( key_event.GetKey() == EKeys::Escape )
    {
        return CancelClicked();
    }

    return FReply::Unhandled();
}

TSharedRef< SWidget > SGitHubToolsPullRequestReview::GenerateWidgetForItemAndColumn( FGithubToolsPullRequestFileInfosPtr item, const FName column_id )
{
    check( item.IsValid() );

    const FMargin row_padding( 3, 0, 0, 0 );

    TSharedPtr< SWidget > item_content_widget;

    if ( column_id == SGitSourceControlReviewFilesWidgetDefs::ColumnID_CheckBoxLabel )
    {
        item_content_widget = SNew( SHorizontalBox ) + SHorizontalBox::Slot()
                                                           .Padding( row_padding )
                                                               [ SNew( SCheckBox )
                                                                   /*.IsChecked( item.Get(), &FGithubToolsPullRequestFileInfos::GetCheckBoxState )
                                                                       .OnCheckStateChanged( item.Get(), &FGithubToolsPullRequestFileInfos::SetCheckBoxState ) */
        ];
    }
    else if ( column_id == SGitSourceControlReviewFilesWidgetDefs::ColumnID_IconLabel )
    {
        item_content_widget = SNew( SHorizontalBox ) + SHorizontalBox::Slot()
                                                           .HAlign( HAlign_Center )
                                                           .VAlign( VAlign_Center )
                                                               [ SNew( SImage )
                                                                       .Image( FRevisionControlStyleManager::Get().GetBrush( item->IconName ) )
                                                                       .ToolTipText( item->ToolTip ) ];
    }
    else if ( column_id == SGitSourceControlReviewFilesWidgetDefs::ColumnID_CommentLabel )
    {
        item_content_widget = SNew( SHorizontalBox ) + SHorizontalBox::Slot()
                                                           .Padding( row_padding )
                                                               [ SNew( SButton )
                                                                       .Text( LOCTEXT( "SourceCOntrol_ManyAssetType", "Comments" ) )
                                                                       .HAlign( HAlign_Center )
                                                                       .IsEnabled( this, &SGitHubToolsPullRequestReview::IsFileCommentsButtonEnabled )
                                                                       .OnClicked( this, &SGitHubToolsPullRequestReview::OnFileCommentsButtonClicked, item ) ];
    }
    else if ( column_id == SGitSourceControlReviewFilesWidgetDefs::ColumnID_AssetLabel )
    {
        item_content_widget = SNew( SHorizontalBox ) + SHorizontalBox::Slot()
                                                           .Padding( row_padding )
                                                               [ SNew( STextBlock )
                                                                       .Text( item->AssetName ) ];
    }
    else if ( column_id == SGitSourceControlReviewFilesWidgetDefs::ColumnID_FileLabel )
    {
        item_content_widget = SNew( SHorizontalBox ) + SHorizontalBox::Slot()
                                                           .Padding( row_padding )
                                                               [ SNew( STextBlock )
                                                                       .Text( item->PackageName ) ];
    }

    return item_content_widget.ToSharedRef();
}

bool SGitHubToolsPullRequestReview::IsFileCommentsButtonEnabled() const
{
    if ( auto * settings = GetDefault< UGitHubToolsSettings >() )
    {
        return !settings->Token.IsEmpty();
    }

    return false;
}

EVisibility SGitHubToolsPullRequestReview::IsWarningPanelVisible() const
{
    if ( auto * settings = GetDefault< UGitHubToolsSettings >() )
    {
        return settings->Token.IsEmpty()
                   ? EVisibility::Visible
                   : EVisibility::Collapsed;
    }

    return EVisibility::Visible;
}

FReply SGitHubToolsPullRequestReview::CancelClicked()
{
    return FReply::Handled();
}

FReply SGitHubToolsPullRequestReview::OnFileCommentsButtonClicked( FGithubToolsPullRequestFileInfosPtr item )
{
    FGitHubToolsModule::DisplayInProgressNotification( LOCTEXT( "SourceControlMenu_InProgress", "Fetching the file comments" ) );

    //FGitHubToolsModule::Get().GetRequestManager().SendRequest< FGitHubToolsHttpRequestData_GetPullRequestNumber, FGitHubToolsHttpResponseData_GetPullRequestNumber >(

    /*const TSharedPtr< SWindow > window = SNew( SWindow )
                                             .Title( LOCTEXT( "SourceCOntrol_ManyAssetType", "Add Comment To Asset On GitHub" ) )
                                             .ClientSize( FVector2D( 400, 200 ) );

    window->SetContent( SNew( SGitHubToolsFileComments )
                            .Item( item )
                            .ParentWindow( window ) );

    const TSharedPtr< SWindow > root_window = FGlobalTabmanager::Get()->GetRootWindow();
    FSlateApplication::Get().AddWindow( window.ToSharedRef() );*/

    return FReply::Handled();
}

TSharedRef< ITableRow > SGitHubToolsPullRequestReview::OnGenerateRowForList( FGithubToolsPullRequestFileInfosPtr file_infos, const TSharedRef< STableViewBase > & owner_table )
{
    TSharedRef< ITableRow > row =
        SNew( SGitSourceControlReviewFilesListRow, owner_table )
            .SourceControlSubmitWidget( SharedThis( this ) )
            .FileInfos( file_infos );

    return row;
}

EColumnSortMode::Type SGitHubToolsPullRequestReview::GetColumnSortMode( const FName column_id ) const
{
    if ( SortByColumn != column_id )
    {
        return EColumnSortMode::None;
    }

    return SortMode;
}

void SGitHubToolsPullRequestReview::OnColumnSortModeChanged( const EColumnSortPriority::Type /*sort_priority*/, const FName & column_id, const EColumnSortMode::Type sort_mode )
{
    SortByColumn = column_id;
    SortMode = sort_mode;

    RequestSort();
}

void SGitHubToolsPullRequestReview::RequestSort()
{
    SortTree();

    ListView->RequestListRefresh();
}

void SGitHubToolsPullRequestReview::SortTree()
{
    if ( SortByColumn == SGitSourceControlReviewFilesWidgetDefs::ColumnID_AssetLabel )
    {
        if ( SortMode == EColumnSortMode::Ascending )
        {
            ListViewItems.Sort( []( const FGithubToolsPullRequestFileInfosPtr & A, const FGithubToolsPullRequestFileInfosPtr & B ) {
                return A->AssetName.ToString() < B->AssetName.ToString();
            } );
        }
        else if ( SortMode == EColumnSortMode::Descending )
        {
            ListViewItems.Sort( []( const FGithubToolsPullRequestFileInfosPtr & A, const FGithubToolsPullRequestFileInfosPtr & B ) {
                return A->AssetName.ToString() >= B->AssetName.ToString();
            } );
        }
    }
    else if ( SortByColumn == SGitSourceControlReviewFilesWidgetDefs::ColumnID_FileLabel )
    {
        if ( SortMode == EColumnSortMode::Ascending )
        {
            ListViewItems.Sort( []( const FGithubToolsPullRequestFileInfosPtr & A, const FGithubToolsPullRequestFileInfosPtr & B ) {
                return A->PackageName.ToString() < B->PackageName.ToString();
            } );
        }
        else if ( SortMode == EColumnSortMode::Descending )
        {
            ListViewItems.Sort( []( const FGithubToolsPullRequestFileInfosPtr & A, const FGithubToolsPullRequestFileInfosPtr & B ) {
                return A->PackageName.ToString() >= B->PackageName.ToString();
            } );
        }
    }
    else if ( SortByColumn == SGitSourceControlReviewFilesWidgetDefs::ColumnID_IconLabel )
    {
        if ( SortMode == EColumnSortMode::Ascending )
        {
            ListViewItems.Sort( []( const FGithubToolsPullRequestFileInfosPtr & A, const FGithubToolsPullRequestFileInfosPtr & B ) {
                return A->IconName.ToString() < B->IconName.ToString();
            } );
        }
        else if ( SortMode == EColumnSortMode::Descending )
        {
            ListViewItems.Sort( []( const FGithubToolsPullRequestFileInfosPtr & A, const FGithubToolsPullRequestFileInfosPtr & B ) {
                return A->IconName.ToString() >= B->IconName.ToString();
            } );
        }
    }
}

void SGitSourceControlReviewFilesListRow::Construct( const FArguments & arguments, const TSharedRef< STableViewBase > & owner_table_view )
{
    SourceControlSubmitWidgetPtr = arguments._SourceControlSubmitWidget;
    FileInfos = arguments._FileInfos;

    SMultiColumnTableRow< FGithubToolsPullRequestFileInfosPtr >::Construct( FSuperRowType::FArguments(), owner_table_view );
}

TSharedRef< SWidget > SGitSourceControlReviewFilesListRow::GenerateWidgetForColumn( const FName & column_name )
{
    if ( const TSharedPtr< SGitHubToolsPullRequestReview > source_control_submit_widget = SourceControlSubmitWidgetPtr.Pin();
         source_control_submit_widget.IsValid() )
    {
        return source_control_submit_widget->GenerateWidgetForItemAndColumn( FileInfos, column_name );
    }

    return SNullWidget::NullWidget;
}

#undef LOCTEXT_NAMESPACE

#endif // SOURCE_CONTROL_WITH_SLATE
