#include "GitHubToolsMenu.h"

#include "GitHubTools.h"
#include "GitHubToolsGitUtils.h"
#include "GitSourceControlModule.h"
#include "HttpRequests/GitHubToolsHttpRequest_GetPullRequestFiles.h"
#include "HttpRequests/GitHubToolsHttpRequest_GetPullRequestInfos.h"
#include "HttpRequests/GitHubToolsHttpRequest_GetPullRequestNumber.h"
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

    FGitHubToolsModule::Get().GetNotificationManager().DisplayInProgressNotification( LOCTEXT( "FetchPRInfos", "Fecthing Pull Request informations" ) );

    FGitHubToolsModule::Get()
        .GetRequestManager()
        .SendRequest< FGitHubToolsHttpRequestData_GetPullRequestNumber >()
        .Then( [ & ]( const TFuture< FGitHubToolsHttpRequestData_GetPullRequestNumber > & result ) {
            const auto & result_data = result.Get();
            const auto pr_number = result_data.GetResult().GetValue();

            if ( pr_number == INDEX_NONE )
            {
                FGitHubToolsModule::Get().GetNotificationManager().DisplayFailureNotification( LOCTEXT( "FetchPrInfosError_NoPRNumber", "Unable to get the PR number" ) );
                return;
            }

            FGitHubToolsModule::Get()
                .GetRequestManager()
                .SendRequest< FGitHubToolsHttpRequestData_GetPullRequestFiles >( pr_number )
                .Then( [ &, pr_number ]( const TFuture< FGitHubToolsHttpRequestData_GetPullRequestFiles > & get_files ) {
                    auto files = get_files.Get();

                    FGitHubToolsModule::Get()
                        .GetRequestManager()
                        .SendRequest< FGitHubToolsHttpRequestData_GetPullRequestInfos >( pr_number )
                        .Then( [ &, files = MoveTemp( files ) ]( const TFuture< FGitHubToolsHttpRequestData_GetPullRequestInfos > & get_pr_infos ) {
                            const auto request = get_pr_infos.Get();
                            if ( request.HasErrorMessage() )
                            {
                                return;
                            }

                            const auto optional_result = request.GetResult();

                            auto pr_infos = optional_result.GetValue();

                            if ( optional_result.IsSet() )
                            {
                                pr_infos->FileInfos.Append( files.GetResult().GetValue() );
                            }

                            FGitHubToolsModule::Get().GetNotificationManager().RemoveInProgressNotification();
                            ShowPullRequestReviewWindow( pr_infos );
                        } );
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
