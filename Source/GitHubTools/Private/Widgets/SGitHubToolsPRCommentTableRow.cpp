#include "SGitHubToolsPRCommentTableRow.h"

#if SOURCE_CONTROL_WITH_SLATE

#define LOCTEXT_NAMESPACE "SGitHubToolsPRCommentTableRow"

void SGitHubToolsPRCommentTableRow::Construct( const FArguments & arguments, const TSharedRef< STableViewBase > & owner_table_view )
{
    Comment = arguments._Comment;

    STableRow< FGithubToolsPullRequestCommentPtr >::Construct(
        STableRow< FGithubToolsPullRequestCommentPtr >::FArguments()
            .Content()
                [ SNew( SBorder )
                        .Padding( FMargin( 5.0f ) )
                            [ SNew( SVerticalBox ) +
                                SVerticalBox::Slot()
                                    .AutoHeight()
                                    .Padding( FMargin( 5.0f ) )
                                        [ SNew( STextBlock )
                                                .AutoWrapText( true )
                                                .Justification( ETextJustify::Type::Center )
                                                .Margin( FMargin( 0.0f, 0.0f, 0.0f, 10.0f ) )
                                                .Visibility( Comment->Path.IsEmpty() ? EVisibility::Collapsed : EVisibility::Visible )
                                                .Text( FText::FromString( Comment->Path ) ) ] +
                                SVerticalBox::Slot()
                                    .AutoHeight()
                                        [ SNew( SHorizontalBox ) +
                                            SHorizontalBox::Slot()
                                                .FillWidth( 1.0f )
                                                .HAlign( EHorizontalAlignment::HAlign_Left )
                                                    [ SNew( STextBlock )
                                                            .Text( Comment->Author ) ] +
                                            SHorizontalBox::Slot()
                                                .AutoWidth()
                                                .HAlign( EHorizontalAlignment::HAlign_Right )
                                                    [ SNew( STextBlock )
                                                            .Text( Comment->Date ) ] ] +
                                SVerticalBox::Slot()
                                    .AutoHeight()
                                    .Padding( FMargin( 5.0f ) )
                                        [ SNew( STextBlock )
                                                .AutoWrapText( true )
                                                .Text( Comment->Comment ) ] ] ],
        owner_table_view );
}

#undef LOCTEXT_NAMESPACE

#endif // SOURCE_CONTROL_WITH_SLATE