#include "SGitHubToolsPRInfosMessageDisplay.h"

#include "SGitHubToolsPRInfosPendingReviews.h"

#if SOURCE_CONTROL_WITH_SLATE

#define LOCTEXT_NAMESPACE "SGitHubToolsPRInfosMessageDisplay"

void SGitHubToolsPRInfosMessageDisplay::Construct( const FArguments & arguments )
{
    PRInfos = arguments._PRInfos.Get();

    TSharedPtr< SBorder > border;

    ChildSlot
        [ SAssignNew( border, SBorder )
                .BorderImage( FCoreStyle::Get().GetBrush( "ErrorReporting.Box" ) )
                .BorderBackgroundColor( FCoreStyle::Get().GetColor( "ErrorReporting.BackgroundColor" ) )
                .HAlign( HAlign_Center ) ];

    if ( PRInfos->State != EGitHubToolsPullRequestsState::Open )
    {
        border->SetContent( SNew( STextBlock )
                                .Justification( ETextJustify::Type::Center )
                                .ColorAndOpacity( FCoreStyle::Get().GetColor( "ErrorReporting.ForegroundColor" ) )
                                .Margin( FMargin( 5.0f ) )
                                .Text( PRInfos->State == EGitHubToolsPullRequestsState::Closed
                                           ? LOCTEXT( "PRClosed", "The PR is closed" )
                                           : LOCTEXT( "PRMerged", "The PR is merged" ) ) );
    }
    else if ( PRInfos->HasPendingReviews() )
    {
        border->SetContent(

            SNew( SHorizontalBox ) +
            SHorizontalBox::Slot()
                .AutoWidth()
                    [ SNew( STextBlock )
                            .Text( LOCTEXT( "PendingReviews", "There are pending reviews" ) )
                            .ColorAndOpacity( FCoreStyle::Get().GetColor( "ErrorReporting.ForegroundColor" ) ) ] +
            SHorizontalBox::Slot()
                .AutoWidth()
                    [ SNew( SSpacer )
                            .Size( FVector2D( 20.0f, 0.0f ) ) ] +
            SHorizontalBox::Slot()
                .AutoWidth()
                    [ SNew( SButton )
                            .VAlign( VAlign_Center )
                            .ButtonColorAndOpacity( FCoreStyle::Get().GetColor( "ErrorReporting.BackgroundColor" ) )
                            .Text( LOCTEXT( "OpenPendingReviewsButtonText", "Open" ) )
                            .OnClicked( this, &SGitHubToolsPRInfosMessageDisplay::OnOpenPendingReviewsClicked ) ] );
    }
}

FReply SGitHubToolsPRInfosMessageDisplay::OnOpenPendingReviewsClicked()
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