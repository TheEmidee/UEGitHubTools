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
                                            .OnClicked( this, &SGitHubToolsPullRequestReview::OpenInGitHubClicked ) ] +
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
                                            .OnCheckStateChanged( this, &SGitHubToolsPullRequestReview::OnShowOnlyUAssetsCheckStateChanged )
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
                                            .OnCheckStateChanged( this, &SGitHubToolsPullRequestReview::OnHideOFPACheckStateChanged )
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
        SHeaderRow::Column( SGitSourceControlReviewFilesWidgetDefs::ColumnID_IconLabel )
            [ SNew( SSpacer ) ]
                .SortMode( this, &SGitHubToolsPullRequestReview::GetColumnSortMode, SGitSourceControlReviewFilesWidgetDefs::ColumnID_IconLabel )
                .OnSort( this, &SGitHubToolsPullRequestReview::OnColumnSortModeChanged )
                .FixedWidth( SGitSourceControlReviewFilesWidgetDefs::IconColumnWidth ) );

    header_row_widget->AddColumn(
        SHeaderRow::Column( SGitSourceControlReviewFilesWidgetDefs::ColumnID_FileLabel )
            .DefaultLabel( LOCTEXT( "FileColumnLabel", "File" ) )
            .SortMode( this, &SGitHubToolsPullRequestReview::GetColumnSortMode, SGitSourceControlReviewFilesWidgetDefs::ColumnID_FileLabel )
            .OnSort( this, &SGitHubToolsPullRequestReview::OnColumnSortModeChanged )
            .FillWidth( 7.0f ) );

    contents->AddSlot()
        .Padding( FMargin( 5, 0 ) )
        .FillHeight( 1.0f )
            [ SNew( SBorder )
                    [ SAssignNew( ListView, SListView< FGithubToolsPullRequestFileInfosPtr > )
                            .ItemHeight( 20 )
                            .ListItemsSource( &PRInfos->FileInfos )
                            .OnGenerateRow( this, &SGitHubToolsPullRequestReview::OnGenerateRowForList )
                            .OnMouseButtonDoubleClick( this, &SGitHubToolsPullRequestReview::OnDiffAgainstRemoteStatusBranchSelected )
                            .HeaderRow( header_row_widget )
                            .SelectionMode( ESelectionMode::Single ) ] ];

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

FReply SGitHubToolsPullRequestReview::OpenInGitHubClicked()
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

void SGitHubToolsPullRequestReview::OnShowOnlyUAssetsCheckStateChanged( ECheckBoxState new_state )
{
    RequestSort();
}

void SGitHubToolsPullRequestReview::OnHideOFPACheckStateChanged( ECheckBoxState new_state )
{
    RequestSort();
}

FReply SGitHubToolsPullRequestReview::OnKeyDown( const FGeometry & my_geometry, const FKeyEvent & key_event )
{
    if ( key_event.GetKey() == EKeys::Escape )
    {
        return CancelClicked();
    }

    return FReply::Unhandled();
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
    //FGitHubToolsModule::DisplayInProgressNotification( LOCTEXT( "SourceControlMenu_InProgress", "Fetching the file comments" ) );

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
            .FileInfos( file_infos )
            .Visibility( MakeAttributeLambda( [ &, file_infos ]() {
                return GetItemRowVisibility( file_infos );
            } ) );

    return row;
}

EVisibility SGitHubToolsPullRequestReview::GetItemRowVisibility( FGithubToolsPullRequestFileInfosPtr file_infos ) const
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
    else if ( SortByColumn == SGitSourceControlReviewFilesWidgetDefs::ColumnID_IconLabel )
    {
        if ( SortMode == EColumnSortMode::Ascending )
        {
            PRInfos->FileInfos.Sort( []( const FGithubToolsPullRequestFileInfosPtr & A, const FGithubToolsPullRequestFileInfosPtr & B ) {
                return A->IconName.ToString() < B->IconName.ToString();
            } );
        }
        else if ( SortMode == EColumnSortMode::Descending )
        {
            PRInfos->FileInfos.Sort( []( const FGithubToolsPullRequestFileInfosPtr & A, const FGithubToolsPullRequestFileInfosPtr & B ) {
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
    const FMargin row_padding( 3, 0, 0, 0 );

    TSharedPtr< SWidget > item_content_widget;

    if ( column_name == SGitSourceControlReviewFilesWidgetDefs::ColumnID_CheckBoxLabel )
    {
        item_content_widget = SNew( SHorizontalBox ) + SHorizontalBox::Slot()
                                                           .Padding( row_padding )
                                                               [ SNew( SCheckBox )
                                                                   /*.IsChecked( item.Get(), &FGithubToolsPullRequestFileInfos::GetCheckBoxState )
                                                                       .OnCheckStateChanged( item.Get(), &FGithubToolsPullRequestFileInfos::SetCheckBoxState ) */
        ];
    }
    else if ( column_name == SGitSourceControlReviewFilesWidgetDefs::ColumnID_IconLabel )
    {
        item_content_widget = SNew( SHorizontalBox ) + SHorizontalBox::Slot()
                                                           .HAlign( HAlign_Center )
                                                           .VAlign( VAlign_Center )
                                                               [ SNew( SImage )
                                                                       .Image( FRevisionControlStyleManager::Get().GetBrush( FileInfos->IconName ) )
                                                                       .ToolTipText( FileInfos->ToolTip ) ];
    }
    else if ( column_name == SGitSourceControlReviewFilesWidgetDefs::ColumnID_FileLabel )
    {
        item_content_widget = SNew( SHorizontalBox ) + SHorizontalBox::Slot()
                                                           .Padding( row_padding )
                                                               [ SNew( STextBlock )
                                                                       .Text( FileInfos->PackageName ) ];
    }

    return item_content_widget.ToSharedRef();
}

#undef LOCTEXT_NAMESPACE

#endif // SOURCE_CONTROL_WITH_SLATE
