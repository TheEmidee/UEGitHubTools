#pragma once

#include "CoreMinimal.h"
#include "GitHubToolsHttpRequestManager.h"
#include "GitHubToolsMenu.h"
#include "GitHubToolsNotificationManager.h"
#include "Modules/ModuleManager.h"

class FToolBarBuilder;
class FMenuBuilder;

class FGitHubToolsModule : public IModuleInterface
{
public:
    void StartupModule() override;
    void ShutdownModule() override;

    FGitHubToolsHttpRequestManager & GetRequestManager() const;
    FGitHubToolsNotificationManager & GetNotificationManager();
    FGitHubToolsMenu & GetMenu();

    static FGitHubToolsModule & Get();

private:
    FGitHubToolsMenu GitHubToolsMenu;
    TUniquePtr< FGitHubToolsHttpRequestManager > HttpRequestManager;
    FGitHubToolsNotificationManager NotificationManager;
};
