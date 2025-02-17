#include "SGitHubToolsPRInfosHeader.h"

#include "GitHubTools.h"
#include "GitHubToolsGitUtils.h"
#include "HttpRequests/GitHubToolsHttpRequest_AddPRReview.h"
#include "HttpRequests/GitHubToolsHttpRequest_MergePR.h"
#include "RevisionControlStyle/RevisionControlStyle.h"

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

    TSharedPtr< SVerticalBox > checks_box = SNew( SVerticalBox );

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
                                .Text( FText::FromString( FString::Printf( TEXT( "State : %s" ), *check->StateStr ) ) )
                                .Justification( ETextJustify::Type::Left ) ] +
                SHorizontalBox::Slot()
                    .AutoWidth()
                    .Padding( FMargin( 10.0f, 0.0f ) )
                        [ SNew( STextBlock )
                                .Text( FText::FromString( check->Description ) )
                                .Justification( ETextJustify::Type::Left ) ] ];
    }

    TSharedPtr< SBorder > checks_tooltip = SNew( SBorder )
        [ checks_box.ToSharedRef() ];

    ChildSlot
        [ SNew( SBorder )
                .Padding( FMargin( 10 ) )
                    [ SNew( SVerticalBox ) +
                        SVerticalBox::Slot()
                            .AutoHeight()
                                [ SNew( STextBlock )
                                        .Text( FText::FromString( FString::Printf( TEXT( "%s ( # %i )" ), *PRInfos->Title, PRInfos->Number ) ) )
                                        .Justification( ETextJustify::Type::Center )
                                        .Font( FAppStyle::GetFontStyle( "BoldFont" ) ) ] +
                        SVerticalBox::Slot()
                            .AutoHeight()
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
                                                                .Justification( ETextJustify::Type::Left )
                                                                .Font( FAppStyle::GetFontStyle( "BoldFont" ) ) ] +
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
                                                                            .Image( GetIcon( PRInfos->bIsMergeable ).GetIcon() ) ] ] +
                                                SVerticalBox::Slot()
                                                    .AutoHeight()
                                                        [ SNew( SHorizontalBox ) +
                                                            SHorizontalBox::Slot()
                                                                .AutoWidth()
                                                                    [ SNew( STextBlock )
                                                                            .Text( FText::FromString( TEXT( "Conversations are resolved :" ) ) )
                                                                            .Justification( ETextJustify::Type::Left ) ] +
                                                            SHorizontalBox::Slot()
                                                                .AutoWidth()
                                                                    [ SNew( SImage )
                                                                            .Image( GetIcon( !PRInfos->bHasUnresolvedConversations ).GetIcon() ) ] ] ] +
                                    SHorizontalBox::Slot()
                                        .AutoWidth()
                                        .Padding( FMargin( 10 ) )
                                            [ SNew( SBorder )
                                                    .BorderBackgroundColor( GitHubToolsUtils::GetPRChecksColor( *PRInfos ) )
                                                    .VAlign( VAlign_Center )
                                                    .HAlign( HAlign_Center )
                                                    .Padding( FMargin( 10.0f ) )
                                                        [ SNew( STextBlock )
                                                                .Text( LOCTEXT( "ChecksText", "Checks" ) )
                                                                .ToolTip( SNew( SToolTip )
                                                                        [ SNew( SBorder )
                                                                                [ checks_tooltip.ToSharedRef() ] ] ) ] ] +
                                    SHorizontalBox::Slot()
                                        .FillWidth( 1.0f ) +
                                    SHorizontalBox::Slot()
                                        .AutoWidth()
                                        .Padding( FMargin( 5.0f ) )
                                            [ SNew( SButton )
                                                    .VAlign( VAlign_Center )
                                                    .Text( LOCTEXT( "ApprovePR", "Approve the PR" ) )
                                                    .OnClicked( this, &SGitHubToolsPRHeader::OnApprovePRClicked ) ] +
                                    SHorizontalBox::Slot()
                                        .AutoWidth()
                                        .Padding( FMargin( 5.0f ) )
                                            [ SNew( SButton )
                                                    .VAlign( VAlign_Center )
                                                    .ButtonColorAndOpacity( FLinearColor( 1.0f, 0.0f, 0.0f, 1.0f ) )
                                                    .Text( LOCTEXT( "MergePR", "Merge the PR" ) )
                                                    .OnClicked( this, &SGitHubToolsPRHeader::OnMergePRClicked ) ] ] ] ];
}

FReply SGitHubToolsPRHeader::OpenInGitHubClicked()
{
    FPlatformProcess::LaunchURL( *PRInfos->URL, nullptr, nullptr );

    return FReply::Handled();
}

FReply SGitHubToolsPRHeader::OnApprovePRClicked()
{
    FGitHubToolsModule::Get().GetNotificationManager().DisplayModalNotification( LOCTEXT( "ApprovePR", "Approving the PR" ) );

    FGitHubToolsModule::Get()
        .GetRequestManager()
        .SendRequest< FGitHubToolsHttpRequestData_AddPRReview >( PRInfos->Id, EGitHubToolsPullRequestReviewEvent::Approve )
        .Then( [ & ]( const TFuture< FGitHubToolsHttpRequestData_AddPRReview > & /*request_future*/ ) {
            FGitHubToolsModule::Get().GetNotificationManager().RemoveModalNotification();
        } );

    return FReply::Handled();
}

FReply SGitHubToolsPRHeader::OnMergePRClicked()
{
    const auto choice = FMessageDialog::Open( EAppMsgType::YesNo, LOCTEXT( "MergePRConfirmation", "Are you sure you want to merge the PR?" ) );
    if ( choice == EAppReturnType::Yes )
    {

        FGitHubToolsModule::Get().GetNotificationManager().DisplayModalNotification( LOCTEXT( "MergePR", "Merging the PR" ) );

        FGitHubToolsModule::Get()
            .GetRequestManager()
            .SendRequest< FGitHubToolsHttpRequest_MergePR >( PRInfos->Id )
            .Then( [ & ]( const TFuture< FGitHubToolsHttpRequest_MergePR > & /*request_future*/ ) {
                FGitHubToolsModule::Get().GetNotificationManager().RemoveModalNotification();
            } );
    }

    return FReply::Handled();
}

EVisibility SGitHubToolsPRHeader::GetPendingReviewsVisibility() const
{
    return PRInfos->HasPendingReviews()
               ? EVisibility::Visible
               : EVisibility::Collapsed;
}

#undef LOCTEXT_NAMESPACE

#endif // SOURCE_CONTROL_WITH_SLATE