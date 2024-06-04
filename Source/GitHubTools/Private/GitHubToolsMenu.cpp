#include "GitHubToolsMenu.h"

#include "GitHubTools.h"
#include "GitHubToolsHttpRequest.h"
#include "GitHubToolsLog.h"
#include "Widgets/SGitHubToolsPullRequestReview.h"

#include <Framework/Notifications/NotificationManager.h>
#include <Widgets/Notifications/SNotificationList.h>

#define LOCTEXT_NAMESPACE "GitHubTools"

namespace
{
    const FName
        MenuTabName( TEXT( "GitHubToolsMenu" ) ),
        ReviewToolName( TEXT( "Review Tool" ) );
}

void FGitHubToolsMenu::Register()
{
    FToolMenuOwnerScoped SourceControlMenuOwner( MenuTabName );
    if ( auto * tool_menus = UToolMenus::Get() )
    {
        auto * source_control_menu = tool_menus->ExtendMenu( "StatusBar.ToolBar.SourceControl" );
        auto & section = source_control_menu->AddSection( "GitHubToolsActions", LOCTEXT( "GitHubToolsMenuHeadingActions", "GitHub Tools" ), FToolMenuInsert( NAME_None, EToolMenuInsertType::First ) );

        AddMenuExtension( section );
    }

    HttpRequestManager = MakeUnique< FGitHubToolsHttpRequestManager >();
}

void FGitHubToolsMenu::Unregister()
{
    if ( auto * tool_menus = UToolMenus::Get() )
    {
        tool_menus->UnregisterOwnerByName( MenuTabName );
    }
}

void FGitHubToolsMenu::ReviewToolButtonMenuEntryClicked()
{
    if ( FGitHubToolsModule::IsOperationInProgress() )
    {
        FMessageLog source_control_log( "SourceControl" );
        source_control_log.Warning( LOCTEXT( "SourceControlMenu_InProgress", "Revision control operation already in progress" ) );
        source_control_log.Notify();
        return;
    }

    FGitHubToolsModule::DisplayInProgressNotification( LOCTEXT( "SourceControlMenu_InProgress", "Fetching the pull request number" ) );

    HttpRequestManager->SendRequest< FGitHubToolsHttpRequestData_GetPullRequestNumber, FGitHubToolsHttpResponseData_GetPullRequestNumber >()
        .Next( [ & ]( const FGitHubToolsHttpResponseData_GetPullRequestNumber & response_data ) {
            const auto pr_number = response_data.GetPullRequestNumber().Get( INDEX_NONE );
            if ( pr_number == INDEX_NONE )
            {
                FGitHubToolsModule::DisplayFailureNotification( LOCTEXT( "GitHubToolslMenu_Failure", "Impossible to find a pull request for the local branch" ) );
                return;
            }

            HttpRequestManager->SendRequest< FGitHubToolsHttpRequestData_GetPullRequestFiles, FGitHubToolsHttpResponseData_GetPullRequestFiles >( pr_number )
                .Next( [ & ]( const FGitHubToolsHttpResponseData_GetPullRequestFiles & get_files_data ) {
                    const auto optional_files = get_files_data.GetPullRequestFiles();
                    if ( !optional_files.IsSet() || optional_files.GetValue().IsEmpty() )
                    {
                        FGitHubToolsModule::DisplayFailureNotification( LOCTEXT( "GitHubToolslMenu_Failure", "Impossible to get the files from the pull request" ) );
                        return;
                    }

                    FGitHubToolsModule::RemoveInProgressNotification();

                    ShowPullRequestReviewWindow( optional_files.GetValue() );
                } );
        } );
}

bool FGitHubToolsMenu::HasGitRemoteUrl() const
{
    return !FGitSourceControlModule::Get().GetProvider().GetRemoteUrl().IsEmpty();
}

void FGitHubToolsMenu::AddMenuExtension( FToolMenuSection & section )
{
    section.AddMenuEntry(
        ReviewToolName,
        LOCTEXT( "GitHubReviewTool", "Open the review tool window" ),
        LOCTEXT( "GitHubReviewToolToolTip", "Open the review tool window" ),
        FSlateIcon( FAppStyle::GetAppStyleSetName(), "SourceControl.Actions.Submit" ),
        FUIAction(
            FExecuteAction::CreateRaw( this, &FGitHubToolsMenu::ReviewToolButtonMenuEntryClicked ),
            FCanExecuteAction::CreateRaw( this, &FGitHubToolsMenu::HasGitRemoteUrl ) ) );
}

void FGitHubToolsMenu::OnReviewWindowDialogClosed( const TSharedRef< SWindow > & window )
{
    ReviewWindowPtr = nullptr;
}

void FGitHubToolsMenu::ShowPullRequestReviewWindow( const TArray< FGithubToolsPullRequestFileInfosPtr > & files )
{
    ReviewWindowPtr = SNew( SWindow )
                          .Title( LOCTEXT( "SourceControlLoginTitle", "Review Window" ) )
                          .ClientSize( FVector2D( 600, 400 ) )
                          .HasCloseButton( true )
                          .SupportsMaximize( true )
                          .SupportsMinimize( true )
                          .SizingRule( ESizingRule::UserSized );

    ReviewWindowPtr->SetOnWindowClosed( FOnWindowClosed::CreateRaw( this, &FGitHubToolsMenu::OnReviewWindowDialogClosed ) );

    const TSharedRef< SGitHubToolsPullRequestReview > SourceControlWidget =
        SNew( SGitHubToolsPullRequestReview )
            .ParentWindow( ReviewWindowPtr )
            .Files( files );

    ReviewWindowPtr->SetContent( SourceControlWidget );

    const TSharedPtr< SWindow > RootWindow = FGlobalTabmanager::Get()->GetRootWindow();
    if ( RootWindow.IsValid() )
    {
        FSlateApplication::Get().AddWindowAsNativeChild( ReviewWindowPtr.ToSharedRef(), RootWindow.ToSharedRef() );
    }
    else
    {
        FSlateApplication::Get().AddWindow( ReviewWindowPtr.ToSharedRef() );
    }
}

#undef LOCTEXT_NAMESPACE
