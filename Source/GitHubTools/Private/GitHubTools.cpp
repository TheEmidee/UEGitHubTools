#include "GitHubTools.h"

#include "GitHubToolsStyle.h"

#include <Misc/MessageDialog.h>
#include <ToolMenus.h>

#define LOCTEXT_NAMESPACE "FGitHubToolsModule"

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

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE( FGitHubToolsModule, GitHubTools )