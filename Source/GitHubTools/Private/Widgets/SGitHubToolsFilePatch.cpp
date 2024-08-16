#include "SGitHubToolsFilePatch.h"

#include "Framework/Text/SlateTextRun.h"
#include "GitHubToolsStyle.h"
#include "SGitHubToolsAddCommentForm.h"
#include "SGitHubToolsPRReviewThreadTableRow.h"

#include <Widgets/Text/SRichTextBlock.h>

#define LOCTEXT_NAMESPACE "SGitHubToolsFilePatch"

namespace SyntaxDecorators
{
    const FString PatchDecorator = TEXT( "patch" );
    const FString AddDecorator = TEXT( "add" );
    const FString RemoveDecorator = TEXT( "remove" );
    const FString NoDecorator = TEXT( "no-decorator" );
}

namespace
{
    class FHeaderViewSyntaxDecorator : public ITextDecorator
    {
    public:
        static TSharedRef< FHeaderViewSyntaxDecorator > Create( FString name, const FSlateColor & color )
        {
            return MakeShareable( new FHeaderViewSyntaxDecorator( MoveTemp( name ), color ) );
        }

        bool Supports( const FTextRunParseResults & run_info, const FString & text ) const override
        {
            return run_info.Name == DecoratorName;
        }

        TSharedRef< ISlateRun > Create( const TSharedRef< class FTextLayout > & text_layout, const FTextRunParseResults & run_parse_result, const FString & original_text, const TSharedRef< FString > & model_text, const ISlateStyle * style ) override
        {
            FRunInfo run_info( run_parse_result.Name );
            for ( const auto & pair : run_parse_result.MetaData )
            {
                run_info.MetaData.Add( pair.Key, original_text.Mid( pair.Value.BeginIndex, pair.Value.EndIndex - pair.Value.BeginIndex ) );
            }

            model_text->Append( original_text.Mid( run_parse_result.ContentRange.BeginIndex, run_parse_result.ContentRange.Len() ) );

            auto text_style = FGitHubToolsStyle::FilePatchTextStyle;
            text_style.SetColorAndOpacity( TextColor );

            return FSlateTextRun::Create( run_info, model_text, text_style );
        }

    private:
        FHeaderViewSyntaxDecorator( FString && name, const FSlateColor & color ) :
            DecoratorName( MoveTemp( name ) ),
            TextColor( color )
        {
        }

        FString DecoratorName;
        FSlateColor TextColor;
    };

}

TSharedRef< SWidget > FGitHubToolsFilePatchViewListItem::GenerateWidgetForItem()
{
    SetReviews();

    return SNew( SBox )
        .HAlign( HAlign_Fill )
        .Padding( FMargin( 4.0f ) )
            [ SNew( SVerticalBox ) +
                SVerticalBox::Slot()
                    .AutoHeight()
                        [ SNew( SHorizontalBox ) +
                            SHorizontalBox::Slot()
                                .AutoWidth()
                                .Padding( 5.0f )
                                    [ SNew( SButton )
                                            .Visibility( DiffSide.IsSet() ? EVisibility::Visible : EVisibility::Hidden )
                                            .Text( LOCTEXT( "AddComment", "+" ) )
                                            .OnClicked( this, &FGitHubToolsFilePatchViewListItem::OnAddCommentClicked ) ] +
                            SHorizontalBox::Slot()
                                .Padding( 5.0f )
                                .AutoWidth()
                                    [ SNew( STextBlock )
                                            .Visibility( DiffSide.Get( EGitHubToolsDiffSide::Unknown ) != EGitHubToolsDiffSide::Right ? EVisibility::Visible : EVisibility::Hidden )
                                            .Text( FText::FromString( FString::FromInt( LeftLine ) ) ) ] +
                            SHorizontalBox::Slot()
                                .AutoWidth()
                                .Padding( 5.0f )
                                    [ SNew( STextBlock )
                                            .Visibility( DiffSide.Get( EGitHubToolsDiffSide::Unknown ) != EGitHubToolsDiffSide::Left ? EVisibility::Visible : EVisibility::Hidden )
                                            .Text( FText::FromString( FString::FromInt( RightLine ) ) ) ] +
                            SHorizontalBox::Slot()
                                .FillWidth( 1.0f )
                                    [ SNew( SRichTextBlock )
                                            .Text( FText::FromString( String ) )
                                            .TextStyle( FAppStyle::Get(), "NormalText" ) +
                                        SRichTextBlock::Decorator( FHeaderViewSyntaxDecorator::Create( SyntaxDecorators::NoDecorator, FLinearColor::White ) ) +
                                        SRichTextBlock::Decorator( FHeaderViewSyntaxDecorator::Create( SyntaxDecorators::PatchDecorator, FStyleColors::AccentBlue.GetSpecifiedColor() ) ) +
                                        SRichTextBlock::Decorator( FHeaderViewSyntaxDecorator::Create( SyntaxDecorators::AddDecorator, FLinearColor::Green ) ) +
                                        SRichTextBlock::Decorator( FHeaderViewSyntaxDecorator::Create( SyntaxDecorators::RemoveDecorator, FLinearColor::Red ) ) ] ] +
                SVerticalBox::Slot()
                    .AutoHeight()
                        [ SNew( SBox )
                                .MaxDesiredHeight( FOptionalSize( 150.0f ) )
                                    [ SAssignNew( AddCommentForm, SGitHubToolsAddCommentForm )
                                            .FileInfos( FileInfos )
                                            .LineInfos( FGitHubToolsAddCommentLineInfos( EGitHubToolsDiffSide::Right, RightLine ) )
                                            .Visibility( EVisibility::Collapsed )
                                            .OnAddCommentDone_Lambda( [ & ]() {
                                                SetReviews();
                                                ThreadList->RequestListRefresh();
                                                AddCommentForm->SetVisibility( EVisibility::Collapsed );
                                            } ) ] ] +
                SVerticalBox::Slot()
                    .AutoHeight()
                        [ SAssignNew( ThreadList, SListView< TSharedPtr< FGithubToolsPullRequestReviewThreadInfos > > )
                                .Visibility( this, &FGitHubToolsFilePatchViewListItem::GetThreadListVisibility )
                                .ListItemsSource( &Reviews )
                                .OnGenerateRow( this, &FGitHubToolsFilePatchViewListItem::GenerateItemRow ) ] ];
}

FGitHubToolsFilePatchViewListItem::FGitHubToolsFilePatchViewListItem( FGithubToolsPullRequestFileInfosPtr file_infos, FString && string, TOptional< EGitHubToolsDiffSide > diff_side, int left_side_line_number, int right_side_line_number ) :
    FileInfos( file_infos ),
    DiffSide( diff_side ),
    String( MoveTemp( string ) ),
    LeftLine( left_side_line_number ),
    RightLine( right_side_line_number )
{
    switch ( DiffSide.Get( EGitHubToolsDiffSide::Unknown ) )
    {
        case EGitHubToolsDiffSide::Left:
        {
            RightLine = INDEX_NONE;
        }
        break;
        case EGitHubToolsDiffSide::Right:
        {
            LeftLine = INDEX_NONE;
        }
        break;
        case EGitHubToolsDiffSide::Unknown:
        {
        }
        break;
    }
}

FReply FGitHubToolsFilePatchViewListItem::OnAddCommentClicked()
{
    AddCommentForm->SetVisibility( EVisibility::Visible );

    return FReply::Handled();
}

TSharedRef< ITableRow > FGitHubToolsFilePatchViewListItem::GenerateItemRow( FGithubToolsPullRequestReviewThreadInfosPtr thread_infos, const TSharedRef< STableViewBase > & owner_table )
{
    return SNew( SGitHubToolsPRReviewThreadTableRow, owner_table )
        //.OnAddCommentClicked( this, &SGitHubToolsPRReviewList::OnAddCommentClicked, item )
        .ThreadInfos( thread_infos );
}

EVisibility FGitHubToolsFilePatchViewListItem::GetThreadListVisibility() const
{
    return Reviews.IsEmpty()
               ? EVisibility::Collapsed
               : EVisibility::Visible;
}

void FGitHubToolsFilePatchViewListItem::SetReviews()
{
    Reviews = FileInfos->Reviews.FilterByPredicate( [ & ]( const FGithubToolsPullRequestReviewThreadInfosPtr & thread_infos ) {
        if ( thread_infos->DiffSide == EGitHubToolsDiffSide::Right )
        {
            return thread_infos->Line == RightLine;
        }

        return thread_infos->Line == LeftLine;
    } );
}

SGitHubToolsFilePatch::~SGitHubToolsFilePatch()
{
}

void SGitHubToolsFilePatch::Construct( const FArguments & arguments )
{
    FileInfos = arguments._FileInfos.Get();

    if ( arguments._ParentWindow.IsValid() )
    {
        WeakParentWindow = arguments._ParentWindow;
    }

    ChildSlot
        [ SNew( SBorder )
                .BorderImage( FAppStyle::GetBrush( "ToolPanel.GroupBorder" ) )
                    [ SNew( SVerticalBox ) +
                        SVerticalBox::Slot()
                            .AutoHeight()
                                [ SNew( STextBlock )
                                        .Text( FText::FromString( FileInfos->Path ) ) ] +
                        SVerticalBox::Slot()
                            .FillHeight( 1.0f )
                            .VAlign( EVerticalAlignment::VAlign_Fill )
                                [ SAssignNew( ListView, SListView< FGitHubToolsFilePatchViewListItemPtr > )
                                        .ListItemsSource( &ListItems )
                                        .OnGenerateRow( this, &SGitHubToolsFilePatch::GenerateRowForItem )
                                        .OnMouseButtonClick( this, &SGitHubToolsFilePatch::OnMouseButtonClick ) ] ] ];

    PopulateListItems();
}

void SGitHubToolsFilePatch::PopulateListItems()
{
    ListItems.Empty();

    TArray< FString > lines;
    const auto line_count = FileInfos->Patch.ParseIntoArray( lines, TEXT( "\n" ), true );

    ListItems.Reserve( line_count );

    auto line_before = 0;
    auto line_after = 0;

    for ( const auto & line : lines )
    {
        auto line_before_increment = 1;
        auto line_after_increment = 1;

        FString decorator;
        const auto first_char = line[ 0 ];

        TOptional< EGitHubToolsDiffSide > diff_side;

        if ( first_char == '@' )
        {
            decorator = SyntaxDecorators::PatchDecorator;

            auto line_copy = line;

            // @@ -3,6 +3,7 @@
            line_copy.RemoveFromStart( TEXT( "@@ -" ) );
            line_copy.RemoveFromEnd( TEXT( " @@" ) );

            const auto first_comma_index = line_copy.Find( TEXT( "," ) );
            const auto before_line_start_str = line_copy.Mid( 0, first_comma_index );

            line_before = FCString::Atoi( GetData( before_line_start_str ) );

            const auto plus_index = line_copy.Find( TEXT( "+" ) );

            line_copy = line_copy.Mid( plus_index + 1 );
            const auto last_comma_index = line_copy.Find( TEXT( "," ) );

            const auto after_line_start_str = line_copy.Mid( 0, last_comma_index );

            line_after = FCString::Atoi( GetData( after_line_start_str ) );

            line_before_increment = 0;
            line_after_increment = 0;
        }
        else if ( first_char == '+' )
        {
            decorator = SyntaxDecorators::AddDecorator;
            diff_side = EGitHubToolsDiffSide::Right;

            line_before_increment = 0;
        }
        else if ( first_char == '-' )
        {
            decorator = SyntaxDecorators::RemoveDecorator;
            diff_side = EGitHubToolsDiffSide::Left;

            line_after_increment = 0;
        }
        else
        {
            decorator = SyntaxDecorators::NoDecorator;
        }

        ListItems.Emplace( MakeShared< FGitHubToolsFilePatchViewListItem >( FileInfos, FString::Printf( TEXT( "<%s>%s</>" ), *decorator, *line ), diff_side, line_before, line_after ) );

        line_before += line_before_increment;
        line_after += line_after_increment;
    }
}

void SGitHubToolsFilePatch::OnMouseButtonClick( TSharedPtr< FGitHubToolsFilePatchViewListItem > item )
{
}

TSharedRef< ITableRow > SGitHubToolsFilePatch::GenerateRowForItem( FGitHubToolsFilePatchViewListItemPtr item, const TSharedRef< STableViewBase > & owner_table ) const
{
    return SNew( STableRow< FGitHubToolsFilePatchViewListItemPtr >, owner_table )
        //.Style(&FBlueprintHeaderViewModule::HeaderViewTableRowStyle)
        .Content()
            [ item->GenerateWidgetForItem() ];
}

#undef LOCTEXT_NAMESPACE