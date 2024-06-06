#include "GitHubToolsMenu.h"

#include "GitHubTools.h"
#include "HttpRequests/GitHubToolsHttpRequest_GetPullRequestComments.h"
#include "HttpRequests/GitHubToolsHttpRequest_GetPullRequestFiles.h"
#include "HttpRequests/GitHubToolsHttpRequest_GetPullRequestNumber.h"
#include "Widgets/SGitHubToolsPullRequestReview.h"

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

    FGitHubToolsModule::Get().GetRequestManager().SendRequest< FGitHubToolsHttpRequestData_GetPullRequestNumber, FGitHubToolsHttpResponseData_GetPullRequestNumber >().Next( [ & ]( const FGitHubToolsHttpResponseData_GetPullRequestNumber & response_data ) {
        const auto pr_number = response_data.GetPullRequestNumber().Get( INDEX_NONE );
        if ( pr_number == INDEX_NONE )
        {
            return;
        }

        FGitHubToolsModule::Get().GetRequestManager().SendRequest< FGitHubToolsHttpRequestData_GetPullRequestFiles, FGitHubToolsHttpResponseData_GetPullRequestFiles >( pr_number ).Next( [ & ]( const FGitHubToolsHttpResponseData_GetPullRequestFiles & get_files_data ) {
            const auto optional_files = get_files_data.GetPullRequestFiles();
            if ( !optional_files.IsSet() || optional_files.GetValue().IsEmpty() )
            {
                return;
            }

            FGitHubToolsModule::Get().GetRequestManager().SendRequest< FGitHubToolsHttpRequestData_GetPullRequestComments, FGitHubToolsHttpResponseData_GetPullRequestComments >( pr_number ).Next( [ & ]( const FGitHubToolsHttpResponseData_GetPullRequestComments & get_comments_data ) {
                const auto optional_comments = get_comments_data.GetPullRequestFiles();
                if ( !optional_comments.IsSet() || optional_comments.GetValue().IsEmpty() )
                {
                    return;
                }

                ShowPullRequestReviewWindow( optional_files.GetValue() );
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
