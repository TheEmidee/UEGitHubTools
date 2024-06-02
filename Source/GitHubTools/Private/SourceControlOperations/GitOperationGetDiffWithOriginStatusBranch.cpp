#include "GitOperationGetDiffWithOriginStatusBranch.h"

#define LOCTEXT_NAMESPACE "GitHubToolsSourceControlOperations"

FName FGitOperationGetDiffWithOriginStatusBranch::GetName() const
{
    return "GetDiffWithOriginStatusBranch";
}

FText FGitOperationGetDiffWithOriginStatusBranch::GetInProgressString() const
{
    return LOCTEXT( "SourceControl_CheckIn", "Getting list of files updated from status branch..." );
}

void FGitOperationGetDiffWithOriginStatusBranch::SetDescription( const FText & description )
{
    Description = description;
}

const FText & FGitOperationGetDiffWithOriginStatusBranch::GetDescription() const
{
    return Description;
}

void FGitOperationGetDiffWithOriginStatusBranch::SetSuccessMessage( const FText & success_message )
{
    SuccessMessage = success_message;
}

const FText & FGitOperationGetDiffWithOriginStatusBranch::GetSuccessMessage() const
{
    return SuccessMessage;
}

void FGitOperationGetDiffWithOriginStatusBranch::AddState( TSharedRef<FGitSourceControlState> state )
{
    Files.Emplace( state );
}

const TArray<TSharedRef<FGitSourceControlState>> & FGitOperationGetDiffWithOriginStatusBranch::GetFiles() const
{
    return Files;
}

#undef LOCTEXT_NAMESPACE
