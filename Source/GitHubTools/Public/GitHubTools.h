#pragma once

#include "GitHubToolsHttpRequestManager.h"
#include "GitHubToolsMenu.h"
#include "GitHubToolsNotificationManager.h"

#include <CoreMinimal.h>
#include <Modules/ModuleManager.h>

class FToolBarBuilder;
class FMenuBuilder;

class FGitHubToolsModule : public IModuleInterface
{
public:
    void StartupModule() override;
    void ShutdownModule() override;

    FGitHubToolsHttpRequestManager & GetRequestManager() const;
    FGitHubToolsNotificationManager & GetNotificationManager();

    static FGitHubToolsModule & Get();

private:
    FGitHubToolsMenu GitSourceControlMenu;
    TUniquePtr< FGitHubToolsHttpRequestManager > HttpRequestManager;
    FGitHubToolsNotificationManager NotificationManager;
};
