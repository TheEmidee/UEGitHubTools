#include "GitHubToolsMenu.h"

#include "GitHubTools.h"
#include "GitHubToolsGitUtils.h"
#include "GitSourceControlModule.h"
#include "HttpRequests/GitHubToolsHttpRequest_GetPullRequestInfos.h"
#include "Widgets/SGitHubToolsPRInfos.h"

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
    if ( FGitHubToolsModule::Get().GetNotificationManager().IsOperationInProgress() )
    {
        FGitHubToolsModule::Get().GetNotificationManager().DisplayFailureNotification( LOCTEXT( "SourceControlMenu_InProgress", "Revision control operation already in progress" ) );
        return;
    }

    FGitHubToolsModule::Get()
        .GetRequestManager()
        .SendRequest< FGitHubToolsHttpRequestData_GetPullRequestInfos, FGitHubToolsHttpResponseData_GetPullRequestInfos >( 573 )
        .Then( [ & ]( const TFuture< FGitHubToolsHttpResponseData_GetPullRequestInfos > & result ) {
            const auto response_data = result.Get();
            const auto pr_infos = response_data.GetPullRequestInfos();

            if ( !pr_infos.IsSet() )
            {
                FGitHubToolsModule::Get()
                    .GetNotificationManager()
                    .DisplayFailureNotification( FText::FromString( FString::Printf( TEXT( "Error while fetching the pull request informations : %s" ), *response_data.GetErrorMessage() ) ) );
                return;
            }
            ShowPullRequestReviewWindow( pr_infos.GetValue() );
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

void FGitHubToolsMenu::ShowPullRequestReviewWindow( const FGithubToolsPullRequestInfosPtr & pr_infos )
{
    ReviewWindowPtr = SNew( SWindow )
                          .Title( LOCTEXT( "SourceControlLoginTitle", "Review Window" ) )
                          .ClientSize( FVector2D( 1280, 1024 ) )
                          .HasCloseButton( true )
                          .SupportsMaximize( true )
                          .SupportsMinimize( true )
                          .SizingRule( ESizingRule::UserSized );

    ReviewWindowPtr->SetOnWindowClosed( FOnWindowClosed::CreateRaw( this, &FGitHubToolsMenu::OnReviewWindowDialogClosed ) );

    const TSharedRef< SGitHubToolsPRInfos > pull_request_review_widget =
        SNew( SGitHubToolsPRInfos )
            .ParentWindow( ReviewWindowPtr )
            .Infos( pr_infos );

    ReviewWindowPtr->SetContent( pull_request_review_widget );

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
