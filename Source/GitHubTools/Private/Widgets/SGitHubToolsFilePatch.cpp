#include "SGitHubToolsFilePatch.h"

#include "Framework/Text/SlateTextRun.h"
#include "GitHubToolsStyle.h"

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
            [ SNew( SRichTextBlock )
                    .Text( FText::FromString( String ) )
                    .TextStyle( FAppStyle::Get(), "NormalText" ) +
                SRichTextBlock::Decorator( FHeaderViewSyntaxDecorator::Create( SyntaxDecorators::NoDecorator, FLinearColor::White ) ) +
                SRichTextBlock::Decorator( FHeaderViewSyntaxDecorator::Create( SyntaxDecorators::PatchDecorator, FLinearColor::Blue ) ) +
                SRichTextBlock::Decorator( FHeaderViewSyntaxDecorator::Create( SyntaxDecorators::AddDecorator, FLinearColor::Green ) ) +
                SRichTextBlock::Decorator( FHeaderViewSyntaxDecorator::Create( SyntaxDecorators::RemoveDecorator, FLinearColor::Red ) ) ];
}

TSharedPtr< FGitHubToolsFilePatchViewListItem > FGitHubToolsFilePatchViewListItem::Create( FString string )
{
    return MakeShareable( new FGitHubToolsFilePatchViewListItem( MoveTemp( string ) ) );
}

FGitHubToolsFilePatchViewListItem::FGitHubToolsFilePatchViewListItem( FString && string ) :
    String( MoveTemp( string ) )
{
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
                                        .OnGenerateRow( this, &SGitHubToolsFilePatch::GenerateRowForItem ) ] ] ];

    PopulateListItems();
}

void SGitHubToolsFilePatch::PopulateListItems()
{
    ListItems.Empty();

    TArray< FString > lines;
    const auto line_count = FileInfos->Patch.ParseIntoArray( lines, TEXT( "\n" ), true );

    ListItems.Reserve( line_count );

    for ( const auto & line : lines )
    {
        FString decorator;
        const auto first_char = line[ 0 ];

        if ( first_char == '@' )
        {
            decorator = SyntaxDecorators::PatchDecorator;
        }
        else if ( first_char == '+' )
        {
            decorator = SyntaxDecorators::AddDecorator;
        }
        else if ( first_char == '-' )
        {
            decorator = SyntaxDecorators::RemoveDecorator;
        }
        else
        {
            decorator = SyntaxDecorators::NoDecorator;
        }

        ListItems.Emplace( FGitHubToolsFilePatchViewListItem::Create( FString::Printf( TEXT( "<%s>%s</>" ), *decorator, *line ) ) );
    }
}

TSharedRef< ITableRow > SGitHubToolsFilePatch::GenerateRowForItem( FGitHubToolsFilePatchViewListItemPtr item, const TSharedRef< STableViewBase > & owner_table ) const
{
    return SNew( STableRow< FGitHubToolsFilePatchViewListItemPtr >, owner_table )
        //.Style(&FBlueprintHeaderViewModule::HeaderViewTableRowStyle)
        .Content()
            [ item->GenerateWidgetForItem() ];
}

#undef LOCTEXT_NAMESPACE