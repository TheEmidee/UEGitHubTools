#include "GitHubToolsMenu.h"

#include "Framework/Notifications/NotificationManager.h"
#include "GitHubToolsLog.h"
#include "GitSourceControl/Public/GitSourceControlModule.h"
#include "SourceControlOperations/GitOperationGetDiffWithOriginStatusBranch.h"
#include "Widgets/SGitHubToolsPullRequestReview.h"
#include "Widgets/Notifications/SNotificationList.h"

#define LOCTEXT_NAMESPACE "GitHubTools"

namespace
{
    const FName
        MenuTabName( TEXT( "GitHubToolsMenu" ) ),
        ReviewToolName( TEXT( "Review Tool" ) );

    TWeakPtr< SNotificationItem > OperationInProgressNotification;
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

void FGitHubToolsMenu::OnGetDiffWithOriginStatusBranchOperationComplete( const TSharedRef< ISourceControlOperation > & source_control_operation, ECommandResult::Type result )
{
    RemoveInProgressNotification();

    if ( result == ECommandResult::Succeeded )
    {
        const auto & operation = static_cast< FGitOperationGetDiffWithOriginStatusBranch & >( source_control_operation.Get() );

        /*if ( operation.GetFiles().IsEmpty() )
        {
            FNotificationInfo info( LOCTEXT( "GitHubToolslMenu_NoDiffForPullRequest", "No files are different from this branch with the status branch" ) );
            info.ExpireDuration = 8.0f;
            FSlateNotificationManager::Get().AddNotification( info );

            return;
        }*/

        ReviewWindowPtr = SNew( SWindow )
                              .Title( LOCTEXT( "SourceControlLoginTitle", "Review Window" ) )
                              .ClientSize( FVector2D( 600, 400 ) )
                              .HasCloseButton( true )
                              .SupportsMaximize( true )
                              .SupportsMinimize( true )
                              .SizingRule( ESizingRule::UserSized );

        // Set the closed callback
        ReviewWindowPtr->SetOnWindowClosed( FOnWindowClosed::CreateRaw( this, &FGitHubToolsMenu::OnReviewWindowDialogClosed ) );

        const TSharedRef< SGitHubToolsPullRequestReview > SourceControlWidget =
            SNew( SGitHubToolsPullRequestReview )
                .ParentWindow( ReviewWindowPtr )
                .Items( operation.GetFiles() );

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
    else
    {
        DisplayFailureNotification( source_control_operation->GetName() );
    }
}

void FGitHubToolsMenu::ReviewToolButtonMenuEntryClicked()
{
    if ( OperationInProgressNotification.IsValid() )
    {
        FMessageLog source_control_log( "SourceControl" );
        source_control_log.Warning( LOCTEXT( "SourceControlMenu_InProgress", "Revision control operation already in progress" ) );
        source_control_log.Notify();
        return;
    }

    const auto operation = ISourceControlOperation::Create< FGitOperationGetDiffWithOriginStatusBranch >();

    if ( const auto result = FGitSourceControlModule::Get().GetProvider().Execute(
             operation,
             EConcurrency::Asynchronous,
             FSourceControlOperationComplete::CreateRaw( this, &FGitHubToolsMenu::OnGetDiffWithOriginStatusBranchOperationComplete ) );
         result == ECommandResult::Succeeded )
    {
        DisplayInProgressNotification( operation->GetInProgressString() );
    }
    else
    {
        DisplayFailureNotification( operation->GetName() );
    }
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

void FGitHubToolsMenu::OnReviewWindowDialogClosed( const TSharedRef<SWindow> & window )
{
    ReviewWindowPtr = nullptr;
}

void FGitHubToolsMenu::DisplayInProgressNotification( const FText & text )
{
    if ( !OperationInProgressNotification.IsValid() )
    {
        FNotificationInfo Info( text );
        Info.bFireAndForget = false;
        Info.ExpireDuration = 0.0f;
        Info.FadeOutDuration = 1.0f;

        OperationInProgressNotification = FSlateNotificationManager::Get().AddNotification( Info );
        if ( OperationInProgressNotification.IsValid() )
        {
            OperationInProgressNotification.Pin()->SetCompletionState( SNotificationItem::CS_Pending );
        }
    }
}

void FGitHubToolsMenu::RemoveInProgressNotification()
{
    if ( OperationInProgressNotification.IsValid() )
    {
        OperationInProgressNotification.Pin()->ExpireAndFadeout();
        OperationInProgressNotification.Reset();
    }
}

void FGitHubToolsMenu::DisplaySucessNotification( FName operation_name )
{
    const auto notification_text = FText::Format( LOCTEXT( "GitHubToolslMenu_Success", "{0} operation was successful!" ), FText::FromName( operation_name ) );
    FNotificationInfo info( notification_text );
    info.bUseSuccessFailIcons = true;
    info.Image = FAppStyle::GetBrush( TEXT( "NotificationList.SuccessImage" ) );

    FSlateNotificationManager::Get().AddNotification( info );

    UE_LOG( LogGitHubTools, Log, TEXT( "%s" ), *notification_text.ToString() );
}

void FGitHubToolsMenu::DisplayFailureNotification( FName operation_name )
{
    const auto notification_text = FText::Format( LOCTEXT( "GitHubToolslMenu_Failure", "Error: {0} operation failed!" ), FText::FromName( operation_name ) );
    FNotificationInfo info( notification_text );
    info.ExpireDuration = 8.0f;
    FSlateNotificationManager::Get().AddNotification( info );

    UE_LOG( LogGitHubTools, Error, TEXT( "%s" ), *notification_text.ToString() );
}

#undef LOCTEXT_NAMESPACE
