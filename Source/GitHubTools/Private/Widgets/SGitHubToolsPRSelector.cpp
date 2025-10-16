#include "SGitHubToolsPRSelector.h"

#include "GitHubTools.h"
#include "GitHubToolsGitUtils.h"
#include "GitSourceControlModule.h"
#include "GitSourceControlUtils.h"
#include "IHotReload.h"
#include "ISourceControlModule.h"
#include "ISourceControlProvider.h"
#include "SourceControlOperations.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Components/HorizontalBox.h"
#include "Dialog/SCustomDialog.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/STextComboBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "GitHubToolsPullRequestReviewWidget"

namespace
{
    FText GetFormattedPRInfos( const FGitHubToolsOpenedPullRequestInfosPtr & PRInfos )
    {
        return FText::Format( LOCTEXT( "PRInfos", "#{0} {1} [ {2} ]" ), PRInfos->Number, FText::FromString( PRInfos->Title ), PRInfos->Author );
    }
}

SGitHubToolsPRSelector::~SGitHubToolsPRSelector()
{
}

void SGitHubToolsPRSelector::Construct( const FArguments & arguments )
{
    OpenedPRs = arguments._OpenedPRs.Get();

    if (auto * current_pr = OpenedPRs.FindByPredicate( []( const FGitHubToolsOpenedPullRequestInfosPtr & pr_infos ) {
        return pr_infos->bIsCurrentPR;
    } ))
    {
        CurrentSelection = *current_pr;
    }

    ChildSlot
    [
        SNew( SBorder )
        .Padding( FMargin( 10 ) )
        [
            SNew( SHorizontalBox )
            +
            SHorizontalBox::Slot()
            .FillWidth( 1.0f )
            [
                SNew( SComboBox<FGitHubToolsOpenedPullRequestInfosPtr> )
                .OptionsSource( &OpenedPRs )
                .OnGenerateWidget( this, &SGitHubToolsPRSelector::OnGenerateComboWidget )
                .OnSelectionChanged( this, &SGitHubToolsPRSelector::OnComboSelectionChanged )
                .InitiallySelectedItem( CurrentSelection )
                [
                    SNew( STextBlock )
                    .Text( this, &SGitHubToolsPRSelector::GetCurrentItemText )
                ]
            ]
            +
            SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew( SButton )
                .Text( FText::FromString( TEXT( "Switch PR" ) ) )
                .OnClicked( this, &SGitHubToolsPRSelector::OnSwitchPRClicked )
                .IsEnabled_Lambda( [ this ]() {
                    return !CurrentSelection->bIsCurrentPR;
                } )
            ]
        ]
    ];
}

TSharedRef< SWidget > SGitHubToolsPRSelector::OnGenerateComboWidget( FGitHubToolsOpenedPullRequestInfosPtr Option )
{
    return
            SNew( STextBlock )
            .Text( GetFormattedPRInfos( Option ) );
}

void SGitHubToolsPRSelector::OnComboSelectionChanged( FGitHubToolsOpenedPullRequestInfosPtr NewSelection, ESelectInfo::Type SelectInfo )
{
    CurrentSelection = NewSelection;
}

FText SGitHubToolsPRSelector::GetCurrentItemText() const
{
    return CurrentSelection.IsValid()
               ? GetFormattedPRInfos( CurrentSelection )
               : FText::FromString( TEXT( "None" ) );
}

FReply SGitHubToolsPRSelector::OnSwitchPRClicked()
{
    TSharedRef< SCustomDialog > Dialog = SNew( SCustomDialog )
        .Title( FText::FromString( TEXT( "Confirm Action" ) ) )
        .Content()
        [
            SNew( STextBlock )
            .Text( FText::FromString( TEXT( "Are you sure you want to checkout another pull request?\n If some files are modified, they will be reverted!" ) ) )
        ]
        .Buttons( {
            SCustomDialog::FButton( FText::FromString( TEXT( "Yes" ) ), FSimpleDelegate::CreateSP( this, &SGitHubToolsPRSelector::CheckoutNewPullRequest ) ),
            SCustomDialog::FButton( FText::FromString( TEXT( "No" ) ) )
        } );

    Dialog->Show();

    return FReply::Handled();
}

void SGitHubToolsPRSelector::CheckoutNewPullRequest()
{
    if (!GitHubToolsUtils::RevertFiles())
    {
        FGitHubToolsModule::Get().GetNotificationManager().DisplayFailureNotification( LOCTEXT( "CantRevertFiles", "Impossible to revert the files" ) );
        return;
    }

    if ( !GitHubToolsUtils::SwitchGitBranch( CurrentSelection->HeadRefName ) )
    {
        return;
    }
    
    FGitHubToolsModule::Get().GetMenu().OpenReviewWindow( true );
}

#undef LOCTEXT_NAMESPACE