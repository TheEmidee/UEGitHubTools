#include "GitHubTools.h"

#include "GitHubToolsStyle.h"
#include "Misc/MessageDialog.h"
#include "ToolMenus.h"

#define LOCTEXT_NAMESPACE "FGitHubToolsModule"

void FGitHubToolsModule::StartupModule()
{
    FGitHubToolsStyle::Initialize();
    FGitHubToolsStyle::ReloadTextures();

    HttpRequestManager = MakeUnique< FGitHubToolsHttpRequestManager >();
    GitHubToolsMenu.Register();
}

void FGitHubToolsModule::ShutdownModule()
{
    HttpRequestManager.Reset();
    GitHubToolsMenu.Unregister();

    UToolMenus::UnRegisterStartupCallback( this );

    UToolMenus::UnregisterOwner( this );

    FGitHubToolsStyle::Shutdown();
}

FGitHubToolsHttpRequestManager & FGitHubToolsModule::GetRequestManager() const
{
    return *HttpRequestManager.Get();
}

FGitHubToolsNotificationManager & FGitHubToolsModule::GetNotificationManager()
{
    return NotificationManager;
}

FGitHubToolsMenu & FGitHubToolsModule::GetMenu()
{
    return GitHubToolsMenu;
}

FGitHubToolsModule & FGitHubToolsModule::Get()
{
    return FModuleManager::Get().LoadModuleChecked< FGitHubToolsModule >( "GitHubTools" );
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE( FGitHubToolsModule, GitHubTools )