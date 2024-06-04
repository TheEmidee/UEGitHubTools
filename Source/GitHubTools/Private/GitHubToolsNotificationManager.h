#pragma once

#include <CoreMinimal.h>

class FGitHubToolsNotificationManager
{
public:
    bool IsOperationInProgress() const;
    void DisplayInProgressNotification( const FText & message );
    void RemoveInProgressNotification();
    void DisplaySucessNotification( const FText & message );
    void DisplayFailureNotification( const FText & error_message );

private:
    TWeakPtr< SNotificationItem > OperationInProgressNotification;
};