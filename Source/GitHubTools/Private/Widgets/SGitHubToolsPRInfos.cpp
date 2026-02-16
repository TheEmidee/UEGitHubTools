#include "SGitHubToolsPRInfos.h"

#include "GitHubToolsGitUtils.h"
#include "GitHubToolsSettings.h"
#include "SGitHubToolsPRConversation.h"
#include "SGitHubToolsPRFilesChanged.h"
#include "SGitHubToolsPRInfosHeader.h"
#include "SGitHubToolsPRInfosMessageDisplay.h"
#include "SGitHubToolsPRSelector.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SWidgetSwitcher.h"
#include "Widgets/Notifications/SErrorText.h"

#define LOCTEXT_NAMESPACE "GitHubToolsPullRequestReviewWidget"

SGitHubToolsPRInfos::~SGitHubToolsPRInfos()
{
}

void SGitHubToolsPRInfos::Construct( const FArguments & arguments )
{
    PRInfos = arguments._Infos.Get();

    TSharedPtr< SVerticalBox > contents;

    ChildSlot
        [ SNew( SBorder )
                .BorderImage( FAppStyle::GetBrush( "ToolPanel.GroupBorder" ) )
                    [ SAssignNew( contents, SVerticalBox ) ] ];

    contents->AddSlot()
        .Padding( FMargin( 5 ) )
        .AutoHeight()
            [ SNew( SGitHubToolsPRSelector )
                    .OpenedPRs( arguments._OpenedPrs.Get() ) ];

    contents->AddSlot()
        .Padding( FMargin( 5 ) )
        .AutoHeight()
            [ SNew( SGitHubToolsPRHeader )
                    .PRInfos( PRInfos ) ];

    contents->AddSlot()
        .Padding( FMargin( 5 ) )
        .FillHeight( 1.0f )
            [ SNew( SVerticalBox ) +
                SVerticalBox::Slot()
                    .AutoHeight()
                        [ SNew( SHorizontalBox ) +
                            SHorizontalBox::Slot()
                                .AutoWidth()
                                    [ SNew( SButton )
                                            .Text( LOCTEXT( "FilesChanged", "Files Changed" ) )
                                            .ContentPadding( FMargin( 5 ) )
                                            .HAlign( HAlign_Center )
                                            .OnClicked_Lambda( [ & ]() {
                                                ActiveTabIndex = 0;
                                                return FReply::Handled();
                                            } ) ] +
                            SHorizontalBox::Slot()
                                .AutoWidth()
                                    [ SNew( SButton )
                                            .Text( LOCTEXT( "Conversation", "Conversation" ) )
                                            .ContentPadding( FMargin( 5 ) )
                                            .HAlign( HAlign_Center )
                                            .OnClicked_Lambda( [ & ]() {
                                                ActiveTabIndex = 1;
                                                return FReply::Handled();
                                            } ) ] ] +
                SVerticalBox::Slot()
                    .FillHeight( 1.0f )
                        [ SAssignNew( TabSwitcher, SWidgetSwitcher )
                                .WidgetIndex_Lambda( [ this ]() {
                                    return ActiveTabIndex;
                                } ) +
                            SWidgetSwitcher::Slot()
                                [ SNew( SGitHubToolsPRFilesChanged )
                                        .PRInfos( PRInfos ) ] +
                            SWidgetSwitcher::Slot()
                                [ SNew( SGitHubToolsPRConversation )
                                        .PRInfos( PRInfos ) ] ] ];

    contents->AddSlot()
        .Padding( FMargin( 5.0f ) )
        .AutoHeight()
            [ SNew( SGitHubToolsPRInfosMessageDisplay )
                    .PRInfos( PRInfos )
                    .Visibility( this, &SGitHubToolsPRInfos::GetMessageDisplayVisibility ) ];

    contents->AddSlot()
        .AutoHeight()
        .Padding( FMargin( 5, 5, 5, 0 ) )
            [ SNew( SBorder )
                    .Visibility( this, &SGitHubToolsPRInfos::IsWarningPanelVisible )
                    .Padding( 5 )
                        [ SNew( SErrorText )
                                .ErrorText( NSLOCTEXT( "GitHubTools.ReviewWindow", "EmptyToken", "You must define the GitHub Token to be able to see and add comments on assets" ) ) ] ];
}

bool SGitHubToolsPRInfos::IsFileCommentsButtonEnabled() const
{
    if ( auto * settings = GetDefault< UGitHubToolsSettings >() )
    {
        return !settings->Token.IsEmpty();
    }

    return false;
}

EVisibility SGitHubToolsPRInfos::IsWarningPanelVisible() const
{
    if ( auto * settings = GetDefault< UGitHubToolsSettings >() )
    {
        return settings->Token.IsEmpty()
                   ? EVisibility::Visible
                   : EVisibility::Collapsed;
    }

    return EVisibility::Visible;
}

EVisibility SGitHubToolsPRInfos::GetMessageDisplayVisibility() const
{
    if ( PRInfos->State != EGitHubToolsPullRequestsState::Open )
    {
        return EVisibility::Visible;
    }

    return EVisibility::Collapsed;
}

#undef LOCTEXT_NAMESPACE