#include "GitHubTools.h"

#include "GitHubToolsStyle.h"

#include <Misc/MessageDialog.h>
#include <ToolMenus.h>

#define LOCTEXT_NAMESPACE "FGitHubToolsModule"

void FGitHubToolsModule::StartupModule()
{
    FGitHubToolsStyle::Initialize();
    FGitHubToolsStyle::ReloadTextures();

    HttpRequestManager = MakeUnique< FGitHubToolsHttpRequestManager >();
    GitSourceControlMenu.Register();
}

void FGitHubToolsModule::ShutdownModule()
{
    HttpRequestManager.Reset();
    GitSourceControlMenu.Unregister();

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

FGitHubToolsModule & FGitHubToolsModule::Get()
{
    return FModuleManager::Get().LoadModuleChecked< FGitHubToolsModule >( "GitHubTools" );
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE( FGitHubToolsModule, GitHubTools )