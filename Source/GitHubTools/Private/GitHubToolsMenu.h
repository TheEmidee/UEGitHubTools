#pragma once

#include "GitHubToolsTypes.h"

#include "CoreMinimal.h"

struct FToolMenuSection;
class SWindow;

class FGitHubToolsMenu
{
public:
    void Register();
    void Unregister();
    void CloseReviewWindow();
    void OpenReviewWindow(bool close_opened_window = false);
    
private:
    void ReviewToolButtonMenuEntryClicked();
    bool HasGitRemoteUrl() const;
    void AddMenuExtension( FToolMenuSection & section );
    void OnReviewWindowDialogClosed( const TSharedRef< SWindow > & window );
    void ShowPullRequestReviewWindow(const FGithubToolsPullRequestInfosPtr & pr_infos, TArray<FGitHubToolsOpenedPullRequestInfosPtr> opened_prs);
    bool ValidateSettings() const;

    TSharedPtr< SWindow > ReviewWindowPtr;
};
