#include "GitHubTools.h"

#include "GitHubToolsStyle.h"
#include "GitSourceControl/Public/GitSourceControlModule.h"
#include "GitSourceControl/Public/GitSourceControlProvider.h"
#include "SourceControlWorkers/GitSourceControlWorkerGetDiffWithOriginStatusBranch.h"

#include <Misc/MessageDialog.h>
#include <ToolMenus.h>

static const FName GitHubToolsTabName( "GitHubTools" );

#define LOCTEXT_NAMESPACE "FGitHubToolsModule"

template < typename Type >
TSharedRef< IGitSourceControlWorker, ESPMode::ThreadSafe > CreateWorker()
{
    return MakeShareable( new Type() );
}

void FGitHubToolsModule::StartupModule()
{
    FGitHubToolsStyle::Initialize();
    FGitHubToolsStyle::ReloadTextures();

    GitSourceControlMenu.Register();

    FGitSourceControlModule::Get().GetProvider().RegisterWorker( "GetDiffWithOriginStatusBranch", FGetGitSourceControlWorker::CreateStatic( &CreateWorker< FGitWorkerGetDiffWithOriginStatusBranch > ) );
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