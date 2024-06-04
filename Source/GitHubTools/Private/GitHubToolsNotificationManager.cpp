#include "GitHubToolsNotificationManager.h"

#include "GitHubToolsLog.h"

#include <Framework/Notifications/NotificationManager.h>
#include <Widgets/Notifications/SNotificationList.h>

bool FGitHubToolsNotificationManager::IsOperationInProgress() const
{
    return OperationInProgressNotification.IsValid();
}

void FGitHubToolsNotificationManager::DisplayInProgressNotification( const FText & message )
{
    if ( !OperationInProgressNotification.IsValid() )
    {
        FNotificationInfo info( message );
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

void FGitHubToolsNotificationManager::RemoveInProgressNotification()
{
    if ( OperationInProgressNotification.IsValid() )
    {
        OperationInProgressNotification.Pin()->ExpireAndFadeout();
        OperationInProgressNotification.Reset();
    }
}

void FGitHubToolsNotificationManager::DisplaySucessNotification( const FText & message )
{
    RemoveInProgressNotification();

    FNotificationInfo info( message );
    info.bUseSuccessFailIcons = true;
    info.Image = FAppStyle::GetBrush( TEXT( "NotificationList.SuccessImage" ) );

    FSlateNotificationManager::Get().AddNotification( info );

    UE_LOG( LogGitHubTools, Log, TEXT( "%s" ), *message.ToString() );
}

void FGitHubToolsNotificationManager::DisplayFailureNotification( const FText & error_message )
{
    RemoveInProgressNotification();

    FNotificationInfo info( error_message );
    info.ExpireDuration = 8.0f;
    FSlateNotificationManager::Get().AddNotification( info );

    UE_LOG( LogGitHubTools, Error, TEXT( "%s" ), *error_message.ToString() );
}