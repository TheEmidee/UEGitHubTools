#include "SGitHubToolsFilePatch.h"

#include "Components/SizeBox.h"
#include "Framework/Text/SlateTextRun.h"
#include "GitHubToolsStyle.h"
#include "SGitHubToolsAddCommentForm.h"

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
                                            .Visibility( bShowAddCommentButton ? EVisibility::Visible : EVisibility::Hidden )
                                            .Text( LOCTEXT( "AddComment", "+" ) )
                                            .OnClicked( this, &FGitHubToolsFilePatchViewListItem::OnAddCommentClicked ) ] +
                            SHorizontalBox::Slot()
                                .Padding( 5.0f )
                                .AutoWidth()
                                    [ SNew( STextBlock )
                                            .Visibility( bShowLineBeforeNumber ? EVisibility::Visible : EVisibility::Hidden )
                                            .Text( FText::FromString( FString::FromInt( LineBefore ) ) ) ] +
                            SHorizontalBox::Slot()
                                .AutoWidth()
                                .Padding( 5.0f )
                                    [ SNew( STextBlock )
                                            .Visibility( bShowLineAfterNumber ? EVisibility::Visible : EVisibility::Hidden )
                                            .Text( FText::FromString( FString::FromInt( LineAfter ) ) ) ] +
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
                    .FillHeight( 1.0f )
                        [ SNew( SBox )
                                .MaxDesiredHeight( FOptionalSize( 150.0f ) )
                                    [ SAssignNew( AddCommentForm, SGitHubToolsAddCommentForm )
                                            .FileInfos( FileInfos )
                                            .LineInfos( FGitHubToolsAddCommentLineInfos( EGitHubToolsDiffSide::Right, LineAfter ) )
                                            .Visibility( EVisibility::Collapsed )
                                            .OnAddCommentDone_Lambda( [ & ]() {
                                                AddCommentForm->SetVisibility( EVisibility::Collapsed );
                                            } ) ] ] ];
}

FGitHubToolsFilePatchViewListItem::FGitHubToolsFilePatchViewListItem( FGithubToolsPullRequestFileInfosPtr file_infos, FString && string, bool show_add_comment_button, bool show_line_before_number, int line_before, bool show_line_after_number, int line_after ) :
    FileInfos( file_infos ),
    bShowAddCommentButton( show_add_comment_button ),
    String( MoveTemp( string ) ),
    bShowLineBeforeNumber( show_line_before_number ),
    LineBefore( line_before ),
    bShowLineAfterNumber( show_line_after_number ),
    LineAfter( line_after )
{
}

FReply FGitHubToolsFilePatchViewListItem::OnAddCommentClicked()
{
    AddCommentForm->SetVisibility( EVisibility::Visible );

    return FReply::Handled();
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

        /*
        if (WeakParentWindow.IsValid())
		{
			WeakParentWindow.Pin()->RequestDestroyWindow();
		}
        */
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
        bool show_add_comment_button = true;
        bool show_line_before_number = true;
        bool show_line_after_number = true;

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

            show_add_comment_button = false;
            show_line_before_number = false;
            show_line_after_number = false;
        }
        else if ( first_char == '+' )
        {
            decorator = SyntaxDecorators::AddDecorator;
            line_before_increment = 0;
            show_line_before_number = false;
        }
        else if ( first_char == '-' )
        {
            decorator = SyntaxDecorators::RemoveDecorator;
            line_after_increment = 0;
            show_line_after_number = false;
        }
        else
        {
            decorator = SyntaxDecorators::NoDecorator;
        }

        ListItems.Emplace( MakeShared< FGitHubToolsFilePatchViewListItem >( FileInfos, FString::Printf( TEXT( "<%s>%s</>" ), *decorator, *line ), show_add_comment_button, show_line_before_number, line_before, show_line_after_number, line_after ) );

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