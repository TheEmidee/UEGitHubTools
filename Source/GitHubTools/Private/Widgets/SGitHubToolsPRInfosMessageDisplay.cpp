#include "SGitHubToolsPRInfosMessageDisplay.h"

#include "Components/HorizontalBox.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SSpacer.h"
#include "Widgets/SWindow.h"
#include "Widgets/Text/STextBlock.h"

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
}

#undef LOCTEXT_NAMESPACE

#endif // SOURCE_CONTROL_WITH_SLATE