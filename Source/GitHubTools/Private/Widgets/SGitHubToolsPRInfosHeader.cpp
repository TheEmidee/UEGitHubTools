#include "SGitHubToolsPRInfosHeader.h"

#include "RevisionControlStyle/RevisionControlStyle.h"
#include "SGitHubToolsPRInfosPendingReviews.h"

#if SOURCE_CONTROL_WITH_SLATE

#define LOCTEXT_NAMESPACE "SGitHubToolsPRHeader"

namespace
{
    FSlateIcon GetIcon( bool value )
    {
        return value
                   ? FSlateIcon( FRevisionControlStyleManager::GetStyleSetName(), "RevisionControl.CheckedOut" )
                   : FSlateIcon( FRevisionControlStyleManager::GetStyleSetName(), "RevisionControl.MarkedForDelete" );
    }
}

void SGitHubToolsPRHeader::Construct( const FArguments & arguments )
{
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
                                        .OnClicked( this, &SGitHubToolsPRHeader::OpenInGitHubClicked ) ] +
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
                                                        [ SNew( SImage )
                                                                .Image( GetIcon( PRInfos->bIsDraft ).GetIcon() ) ] ] +
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
                                                        [ SNew( SImage )
                                                                .Image( GetIcon( PRInfos->bIsMergeable ).GetIcon() ) ] ] ] +
                        SHorizontalBox::Slot()
                            .AutoWidth()
                            .Padding( FMargin( 10 ) )
                                [ SAssignNew( checks_box, SVerticalBox ) ] +
                        SHorizontalBox::Slot()
                            .AutoWidth()
                            .Padding( FMargin( 10 ) )
                                [ SNew( SBorder )
                                        .Padding( FMargin( 10.0f ) )
                                        .BorderBackgroundColor( FCoreStyle::Get().GetColor( "ErrorReporting.BackgroundColor" ) )
                                        .Visibility( this, &SGitHubToolsPRHeader::GetPendingReviewsVisibility )
                                            [ SNew( SButton )
                                                    .VAlign( VAlign_Center )
                                                    .ButtonColorAndOpacity( FCoreStyle::Get().GetColor( "ErrorReporting.BackgroundColor" ) )
                                                    .Text( LOCTEXT( "OpenPendingReviewsButtonText", "Open Pending Reviews" ) )
                                                    .OnClicked( this, &SGitHubToolsPRHeader::OnOpenPendingReviewsClicked ) ] ] ] ];

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

FReply SGitHubToolsPRHeader::OpenInGitHubClicked()
{
    FPlatformProcess::LaunchURL( *PRInfos->URL, nullptr, nullptr );

    return FReply::Handled();
}

EVisibility SGitHubToolsPRHeader::GetPendingReviewsVisibility() const
{
    return PRInfos->HasPendingReviews()
               ? EVisibility::Visible
               : EVisibility::Collapsed;
}

FReply SGitHubToolsPRHeader::OnOpenPendingReviewsClicked()
{
    PendingReviewsWindow = SNew( SWindow )
                               .Title( LOCTEXT( "SourceControlLoginTitle", "Pending Reviews" ) )
                               .ClientSize( FVector2D( 1024, 768 ) )
                               .HasCloseButton( true )
                               .SupportsMaximize( false )
                               .SupportsMinimize( false )
                               .SizingRule( ESizingRule::FixedSize );

    PendingReviewsWindow->SetOnWindowClosed( FOnWindowClosed::CreateLambda( [ & ]( const TSharedRef< SWindow > & /*window*/ ) {
        PendingReviewsWindow = nullptr;
        
    } ) );

    PendingReviewsWindow->SetContent( SNew( SGitHubToolsPRInfosPendingReviews )
                                          .ParentWindow( PendingReviewsWindow )
                                          .PRInfos( PRInfos ) );

    if ( const TSharedPtr< SWindow > root_window = FGlobalTabmanager::Get()->GetRootWindow();
         root_window.IsValid() )
    {
        FSlateApplication::Get().AddModalWindow( PendingReviewsWindow.ToSharedRef(), root_window );
    }

    return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE

#endif // SOURCE_CONTROL_WITH_SLATE