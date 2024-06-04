#include "GitHubTools.h"

#include "GitHubToolsLog.h"
#include "GitHubToolsStyle.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"

#include <Misc/MessageDialog.h>
#include <ToolMenus.h>

#define LOCTEXT_NAMESPACE "FGitHubToolsModule"

namespace
{
    TWeakPtr< SNotificationItem > OperationInProgressNotification;
}

void FGitHubToolsModule::StartupModule()
{
    FGitHubToolsStyle::Initialize();
    FGitHubToolsStyle::ReloadTextures();

    GitSourceControlMenu.Register();
}

void FGitHubToolsModule::ShutdownModule()
{
    GitSourceControlMenu.Unregister();

    UToolMenus::UnRegisterStartupCallback( this );

    UToolMenus::UnregisterOwner( this );

    FGitHubToolsStyle::Shutdown();
}

bool FGitHubToolsModule::IsOperationInProgress()
{
    return OperationInProgressNotification.IsValid();
}

void FGitHubToolsModule::DisplayInProgressNotification( const FText & text )
{
    if ( !OperationInProgressNotification.IsValid() )
    {
        FNotificationInfo info( text );
        info.bFireAndForget = false;
        info.ExpireDuration = 0.0f;
        info.FadeOutDuration = 1.0f;

        OperationInProgressNotification = FSlateNotificationManager::Get().AddNotification( info );
        if ( OperationInProgressNotification.IsValid() )
        {
            OperationInProgressNotification.Pin()->SetCompletionState( SNotificationItem::CS_Pending );
        }
    }
}

void FGitHubToolsModule::RemoveInProgressNotification()
{
    if ( OperationInProgressNotification.IsValid() )
    {
        OperationInProgressNotification.Pin()->ExpireAndFadeout();
        OperationInProgressNotification.Reset();
    }
}

void FGitHubToolsModule::DisplaySucessNotification( FName operation_name )
{
    RemoveInProgressNotification();

    const auto notification_text = FText::Format( LOCTEXT( "GitHubToolslMenu_Success", "{0} operation was successful!" ), FText::FromName( operation_name ) );

    FNotificationInfo info( notification_text );
    info.bUseSuccessFailIcons = true;
    info.Image = FAppStyle::GetBrush( TEXT( "NotificationList.SuccessImage" ) );

    FSlateNotificationManager::Get().AddNotification( info );

    UE_LOG( LogGitHubTools, Log, TEXT( "%s" ), *notification_text.ToString() );
}

void FGitHubToolsModule::DisplayFailureNotification( const FText & error_message )
{
    RemoveInProgressNotification();

    FNotificationInfo info( error_message );
    info.ExpireDuration = 8.0f;
    FSlateNotificationManager::Get().AddNotification( info );

    UE_LOG( LogGitHubTools, Error, TEXT( "%s" ), *error_message.ToString() );
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE( FGitHubToolsModule, GitHubTools )