#pragma once

#include "CoreMinimal.h"

struct FScopedSlowTask;
class SNotificationItem;

class FGitHubToolsNotificationManager
{
public:
    bool IsOperationInProgress() const;
    void DisplayInProgressNotification( const FText & message );
    void RemoveInProgressNotification();
    void DisplaySuccessNotification( const FText & message );
    void DisplayFailureNotification( const FText & error_message );

    void DisplayModalNotification( const FText & message );
    void RemoveModalNotification();

private:
    TWeakPtr< SNotificationItem > OperationInProgressNotification;
    TSharedPtr< FScopedSlowTask > SlowTaskPtr;
    TSharedPtr< class SWindow > WindowPtr;
};