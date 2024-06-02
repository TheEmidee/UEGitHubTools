#pragma once

#include <CoreMinimal.h>
#include <ISourceControlProvider.h>

class FGitHubToolsMenu
{
public:
    void Register();
    void Unregister();

private:
    void OnGetDiffWithOriginStatusBranchOperationComplete( const TSharedRef< ISourceControlOperation > & source_control_operation, ECommandResult::Type result );
    void ReviewToolButtonMenuEntryClicked();
    bool HasGitRemoteUrl() const;
    void AddMenuExtension( FToolMenuSection & section );
    void OnReviewWindowDialogClosed( const TSharedRef< SWindow > & window );

    static void DisplayInProgressNotification( const FText & text );
    static void RemoveInProgressNotification();
    static void DisplaySucessNotification( FName operation_name );
    static void DisplayFailureNotification( FName operation_name );

    TSharedPtr< SWindow > ReviewWindowPtr;
};
