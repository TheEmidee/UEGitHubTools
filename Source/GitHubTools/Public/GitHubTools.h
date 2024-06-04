#pragma once

#include "GitHubToolsMenu.h"

#include <CoreMinimal.h>
#include <Modules/ModuleManager.h>

class FToolBarBuilder;
class FMenuBuilder;

class FGitHubToolsModule : public IModuleInterface
{
public:
    void StartupModule() override;
    void ShutdownModule() override;

    static bool IsOperationInProgress();
    static void DisplayInProgressNotification( const FText & text );
    static void RemoveInProgressNotification();
    static void DisplaySucessNotification( FName operation_name );
    static void DisplayFailureNotification( const FText & error_message );

private:
    FGitHubToolsMenu GitSourceControlMenu;
};
