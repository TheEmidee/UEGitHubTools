#include "SGitHubToolsPRInfosPendingReviews.h"

#include "RevisionControlStyle/RevisionControlStyle.h"

#if SOURCE_CONTROL_WITH_SLATE

#define LOCTEXT_NAMESPACE "SGitHubToolsPRInfosPendingReviews"

void SGitHubToolsPRInfosPendingReviews::Construct( const FArguments & arguments )
{
    ParentFrame = arguments._ParentWindow.Get();
    PRInfos = arguments._PRInfos.Get();

    TSharedPtr< SVerticalBox > checks_box;

    ChildSlot
        [ SNew( SBorder )
                .Padding( FMargin( 10 ) )
                    [ SNew( SHorizontalBox ) +
                        SHorizontalBox::Slot()
                            .AutoWidth()
                            .Padding( FMargin( 10 ) )
                                [ SNew( SButton )
                                        .VAlign( VAlign_Center )
                                        .Text( LOCTEXT( "OpenInGitHub", "Open in GitHub" ) )
                                        .OnClicked( this, &SGitHubToolsPRInfosPendingReviews::OpenInGitHubClicked ) ] +
                        SHorizontalBox::Slot()
                            .AutoWidth()
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
                                                    .Justification( ETextJustify::Type::Left ) ] +
                                    SVerticalBox::Slot()
                                        .AutoHeight()
                                            [ SNew( STextBlock )
                                                    .Text( FText::FromString( FString::Printf( TEXT( "%s -> %s" ), *PRInfos->HeadRefName, *PRInfos->BaseRefName ) ) )
                                                    .Justification( ETextJustify::Type::Left ) ] +
                                    SVerticalBox::Slot()
                                        .AutoHeight()
                                            [ SNew( STextBlock )
                                                    .Text( FText::FromString( PRInfos->CreatedAt ) )
                                                    .Justification( ETextJustify::Type::Left ) ] ] +
                        SHorizontalBox::Slot()
                            .AutoWidth()
                            .Padding( FMargin( 10 ) )
                                [ SNew( SVerticalBox ) +
                                    SVerticalBox::Slot()
                                        .AutoHeight()
                                            [ SNew( STextBlock )
                                                    .Text( FText::FromString( FString::Printf( TEXT( "Changed files : %i" ), PRInfos->ChangedFiles ) ) )
                                                    .Justification( ETextJustify::Type::Left ) ] +
                                    SVerticalBox::Slot()
                                        .AutoHeight()
                                            [ SNew( STextBlock )
                                                    .Text( FText::FromString( FString::Printf( TEXT( "Commits : %i" ), PRInfos->CommitCount ) ) )
                                                    .Justification( ETextJustify::Type::Left ) ] +
                                    SVerticalBox::Slot()
                                        .AutoHeight()
                                            [ SNew( SHorizontalBox ) +
                                                SHorizontalBox::Slot()
                                                    .AutoWidth()
                                                        [ SNew( STextBlock )
                                                                .Text( FText::FromString( TEXT( "Draft :" ) ) )
                                                                .Justification( ETextJustify::Type::Left ) ] +
                                                SHorizontalBox::Slot()
                                                    .AutoWidth()
                                                        [ SNew( SImage ) ] ] +
                                    SVerticalBox::Slot()
                                        .AutoHeight()
                                            [ SNew( SHorizontalBox ) +
                                                SHorizontalBox::Slot()
                                                    .AutoWidth()
                                                        [ SNew( STextBlock )
                                                                .Text( FText::FromString( TEXT( "Mergeable :" ) ) )
                                                                .Justification( ETextJustify::Type::Left ) ] +
                                                SHorizontalBox::Slot()
                                                    .AutoWidth()
                                                        [ SNew( SImage ) ] ] ] +
                        SHorizontalBox::Slot()
                            .AutoWidth()
                            .Padding( FMargin( 10 ) )
                                [ SAssignNew( checks_box, SVerticalBox ) ] ] ];

    checks_box->AddSlot()
        [ SNew( STextBlock )
                .Text( FText::FromString( TEXT( "Checks : " ) ) )
                .Justification( ETextJustify::Type::Left ) ];

    for ( auto check : PRInfos->Checks )
    {
        checks_box->AddSlot()
            [ SNew( SHorizontalBox ) +
                SHorizontalBox::Slot()
                    .AutoWidth()
                    .Padding( FMargin( 10.0f, 0.0f ) )
                        [ SNew( STextBlock )
                                .Text( FText::FromString( check->Context ) )
                                .Justification( ETextJustify::Type::Left ) ] +
                SHorizontalBox::Slot()
                    .AutoWidth()
                    .Padding( FMargin( 10.0f, 0.0f ) )
                        [ SNew( STextBlock )
                                .Text( FText::FromString( FString::Printf( TEXT( "State : %s" ), *check->State ) ) )
                                .Justification( ETextJustify::Type::Left ) ] +
                SHorizontalBox::Slot()
                    .AutoWidth()
                    .Padding( FMargin( 10.0f, 0.0f ) )
                        [ SNew( STextBlock )
                                .Text( FText::FromString( check->Description ) )
                                .Justification( ETextJustify::Type::Left ) ] ];
    }
}

FReply SGitHubToolsPRInfosPendingReviews::OpenInGitHubClicked()
{
    FPlatformProcess::LaunchURL( *PRInfos->URL, nullptr, nullptr );

    return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE

#endif // SOURCE_CONTROL_WITH_SLATE