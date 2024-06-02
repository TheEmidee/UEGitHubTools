#include "SGitHubToolsPullRequestReview.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "GitHubToolsGitUtils.h"
#include "GitHubToolsReviewFileItem.h"
#include "GitHubToolsSettings.h"
#include "GitSourceControlModule.h"
#include "RevisionControlStyle/RevisionControlStyle.h"
#include "SGitHubToolsAddCommentToFile.h"

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

    for ( const auto & item : arguments._Items.Get() )
    {
        ListViewItems.Add( MakeShareable( new FGitSourceControlReviewFileItem( item ) ) );
    }

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
                    [ SAssignNew( ListView, SListView< TSharedPtr< FGitSourceControlReviewFileItem > > )
                            .ItemHeight( 20 )
                            .ListItemsSource( &ListViewItems )
                            .OnGenerateRow( this, &SGitHubToolsPullRequestReview::OnGenerateRowForList )
                            .OnMouseButtonDoubleClick( this, &SGitHubToolsPullRequestReview::OnDiffAgainstRemoteStatusBranchSelected )
                            .HeaderRow( header_row_widget )
                            .SelectionMode( ESelectionMode::Multi ) ] ];

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

void SGitHubToolsPullRequestReview::OnDiffAgainstRemoteStatusBranchSelected( TSharedPtr< FGitSourceControlReviewFileItem > selected_item )
{
    if ( selected_item->FileState->State.FileState == EFileState::Added )
    {
        const auto asset_data = GitHubToolsGitUtils::GetAssetDataFromState( selected_item->FileState );
        if ( asset_data.IsSet() )
        {
            const auto & asset_tools_module = FModuleManager::GetModuleChecked< FAssetToolsModule >( "AssetTools" );
            asset_tools_module.Get().OpenEditorForAssets( { asset_data.GetValue().GetAsset() } );
            selected_item->SetCheckBoxState( ECheckBoxState::Checked );
            return;
        }
    }

    if ( selected_item->FileState->State.FileState == EFileState::Modified )
    {
        GitHubToolsGitUtils::DiffAssetAgainstOriginStatusBranch( selected_item->FileState );
        selected_item->SetCheckBoxState( ECheckBoxState::Checked );
    }
}

FReply SGitHubToolsPullRequestReview::OnKeyDown( const FGeometry & my_geometry, const FKeyEvent & key_event )
{
    // Pressing escape returns as if the user clicked cancel
    if ( key_event.GetKey() == EKeys::Escape )
    {
        return CancelClicked();
    }

    return FReply::Unhandled();
}

TSharedRef< SWidget > SGitHubToolsPullRequestReview::GenerateWidgetForItemAndColumn( TSharedPtr< FGitSourceControlReviewFileItem > item, const FName column_id )
{
    check( item.IsValid() );

    const FMargin row_padding( 3, 0, 0, 0 );

    TSharedPtr< SWidget > item_content_widget;

    if ( column_id == SGitSourceControlReviewFilesWidgetDefs::ColumnID_CheckBoxLabel )
    {
        item_content_widget = SNew( SHorizontalBox ) + SHorizontalBox::Slot()
                                                           .Padding( row_padding )
                                                               [ SNew( SCheckBox )
                                                                       .IsChecked( item.Get(), &FGitSourceControlReviewFileItem::GetCheckBoxState )
                                                                       .OnCheckStateChanged( item.Get(), &FGitSourceControlReviewFileItem::SetCheckBoxState ) ];
    }
    else if ( column_id == SGitSourceControlReviewFilesWidgetDefs::ColumnID_IconLabel )
    {
        item_content_widget = SNew( SHorizontalBox ) + SHorizontalBox::Slot()
                                                           .HAlign( HAlign_Center )
                                                           .VAlign( VAlign_Center )
                                                               [ SNew( SImage )
                                                                       .Image( FRevisionControlStyleManager::Get().GetBrush( item->GetIconName() ) )
                                                                       .ToolTipText( item->GetIconTooltip() ) ];
    }
    else if ( column_id == SGitSourceControlReviewFilesWidgetDefs::ColumnID_CommentLabel )
    {
        item_content_widget = SNew( SHorizontalBox ) + SHorizontalBox::Slot()
                                                           .Padding( row_padding )
                                                               [ SNew( SButton )
                                                                       .Text( LOCTEXT( "SourceCOntrol_ManyAssetType", "Add Comment" ) )
                                                                       .HAlign( HAlign_Center )
                                                                       .IsEnabled( this, &SGitHubToolsPullRequestReview::IsAddCommentButtonEnabled )
                                                                       .OnClicked( this, &SGitHubToolsPullRequestReview::OnAddCommentButtonClicked, item ) ];
    }
    else if ( column_id == SGitSourceControlReviewFilesWidgetDefs::ColumnID_AssetLabel )
    {
        item_content_widget = SNew( SHorizontalBox ) + SHorizontalBox::Slot()
                                                           .Padding( row_padding )
                                                               [ SNew( STextBlock )
                                                                       .Text( item->GetAssetName() ) ];
    }
    else if ( column_id == SGitSourceControlReviewFilesWidgetDefs::ColumnID_FileLabel )
    {
        item_content_widget = SNew( SHorizontalBox ) + SHorizontalBox::Slot()
                                                           .Padding( row_padding )
                                                               [ SNew( STextBlock )
                                                                       .Text( item->GetPackageName() )
                                                                       .ToolTipText( item->GetFileName() ) ];
    }

    return item_content_widget.ToSharedRef();
}

bool SGitHubToolsPullRequestReview::IsAddCommentButtonEnabled() const
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

FReply SGitHubToolsPullRequestReview::OnAddCommentButtonClicked( TSharedPtr< FGitSourceControlReviewFileItem > item )
{
    const TSharedPtr< SWindow > window = SNew( SWindow )
                                             .Title( LOCTEXT( "SourceCOntrol_ManyAssetType", "Add Comment To Asset On GitHub" ) )
                                             .ClientSize( FVector2D( 400, 200 ) );

    window->SetContent( SNew( SGitHubToolsAddCommentToFile )
                            .Item( item )
                            .ParentWindow( window ) );

    const TSharedPtr< SWindow > root_window = FGlobalTabmanager::Get()->GetRootWindow();
    FSlateApplication::Get().AddModalWindow( window.ToSharedRef(), root_window );

    return FReply::Handled();
}

TSharedRef< ITableRow > SGitHubToolsPullRequestReview::OnGenerateRowForList( TSharedPtr< FGitSourceControlReviewFileItem > submit_item, const TSharedRef< STableViewBase > & owner_table )
{
    TSharedRef< ITableRow > row =
        SNew( SGitSourceControlReviewFilesListRow, owner_table )
            .SourceControlSubmitWidget( SharedThis( this ) )
            .Item( submit_item );

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
    // Sort the list of root items
    SortTree();

    ListView->RequestListRefresh();
}

void SGitHubToolsPullRequestReview::SortTree()
{
    if ( SortByColumn == SGitSourceControlReviewFilesWidgetDefs::ColumnID_AssetLabel )
    {
        if ( SortMode == EColumnSortMode::Ascending )
        {
            ListViewItems.Sort( []( const TSharedPtr< FGitSourceControlReviewFileItem > & A, const TSharedPtr< FGitSourceControlReviewFileItem > & B ) {
                return A->GetAssetName().ToString() < B->GetAssetName().ToString();
            } );
        }
        else if ( SortMode == EColumnSortMode::Descending )
        {
            ListViewItems.Sort( []( const TSharedPtr< FGitSourceControlReviewFileItem > & A, const TSharedPtr< FGitSourceControlReviewFileItem > & B ) {
                return A->GetAssetName().ToString() >= B->GetAssetName().ToString();
            } );
        }
    }
    else if ( SortByColumn == SGitSourceControlReviewFilesWidgetDefs::ColumnID_FileLabel )
    {
        if ( SortMode == EColumnSortMode::Ascending )
        {
            ListViewItems.Sort( []( const TSharedPtr< FGitSourceControlReviewFileItem > & A, const TSharedPtr< FGitSourceControlReviewFileItem > & B ) {
                return A->GetPackageName().ToString() < B->GetPackageName().ToString();
            } );
        }
        else if ( SortMode == EColumnSortMode::Descending )
        {
            ListViewItems.Sort( []( const TSharedPtr< FGitSourceControlReviewFileItem > & A, const TSharedPtr< FGitSourceControlReviewFileItem > & B ) {
                return A->GetPackageName().ToString() >= B->GetPackageName().ToString();
            } );
        }
    }
    else if ( SortByColumn == SGitSourceControlReviewFilesWidgetDefs::ColumnID_IconLabel )
    {
        if ( SortMode == EColumnSortMode::Ascending )
        {
            ListViewItems.Sort( []( const TSharedPtr< FGitSourceControlReviewFileItem > & A, const TSharedPtr< FGitSourceControlReviewFileItem > & B ) {
                return A->GetIconName().ToString() < B->GetIconName().ToString();
            } );
        }
        else if ( SortMode == EColumnSortMode::Descending )
        {
            ListViewItems.Sort( []( const TSharedPtr< FGitSourceControlReviewFileItem > & A, const TSharedPtr< FGitSourceControlReviewFileItem > & B ) {
                return A->GetIconName().ToString() >= B->GetIconName().ToString();
            } );
        }
    }
}

void SGitSourceControlReviewFilesListRow::Construct( const FArguments & arguments, const TSharedRef< STableViewBase > & owner_table_view )
{
    SourceControlSubmitWidgetPtr = arguments._SourceControlSubmitWidget;
    Item = arguments._Item;

    SMultiColumnTableRow< TSharedPtr< FGitSourceControlReviewFileItem > >::Construct( FSuperRowType::FArguments(), owner_table_view );
}

TSharedRef< SWidget > SGitSourceControlReviewFilesListRow::GenerateWidgetForColumn( const FName & column_name )
{
    // Create the widget for this item
    if ( const TSharedPtr< SGitHubToolsPullRequestReview > source_control_submit_widget = SourceControlSubmitWidgetPtr.Pin();
         source_control_submit_widget.IsValid() )
    {
        return source_control_submit_widget->GenerateWidgetForItemAndColumn( Item, column_name );
    }

    // Packages dialog no longer valid; return a valid, null widget.
    return SNullWidget::NullWidget;
}

#undef LOCTEXT_NAMESPACE

#endif // SOURCE_CONTROL_WITH_SLATE
