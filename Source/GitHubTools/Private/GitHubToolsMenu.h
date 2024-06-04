#pragma once

#include "GitHubToolsHttpRequest.h"

#include <CoreMinimal.h>
#include <ISourceControlProvider.h>

class FGitHubToolsMenu
{
public:
    void Register();
    void Unregister();

private:
    void ReviewToolButtonMenuEntryClicked();
    bool HasGitRemoteUrl() const;
    void AddMenuExtension( FToolMenuSection & section );
    void OnReviewWindowDialogClosed( const TSharedRef< SWindow > & window );
    void ShowPullRequestReviewWindow( const TArray< FGithubToolsPullRequestFileInfosPtr > & files );

    TSharedPtr< SWindow > ReviewWindowPtr;
    TUniquePtr< FGitHubToolsHttpRequestManager > HttpRequestManager;
};
