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

private:
    FGitHubToolsMenu GitSourceControlMenu;
};
