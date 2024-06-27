#pragma once

#include "GitHubToolsTypes.h"

#include <CoreMinimal.h>

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
    void ShowPullRequestReviewWindow( const FGithubToolsPullRequestInfosPtr & pr_infos );
    bool ValidateSettings() const;

    TSharedPtr< SWindow > ReviewWindowPtr;
};
