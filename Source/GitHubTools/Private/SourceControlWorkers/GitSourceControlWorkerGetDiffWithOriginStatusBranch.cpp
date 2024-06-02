#include "GitSourceControlWorkerGetDiffWithOriginStatusBranch.h"

#include "GitSourceControl/Public/GitSourceControlCommand.h"
#include "GitSourceControl/Public/GitSourceControlModule.h"
#include "GitSourceControl/Public/GitSourceControlState.h"
#include "GitSourceControl/Public/GitSourceControlUtils.h"
#include "SourceControlOperations/GitOperationGetDiffWithOriginStatusBranch.h"

#define LOCTEXT_NAMESPACE "GitHubToolsSourceControlOperations"

FName FGitWorkerGetDiffWithOriginStatusBranch::GetName() const
{
    return TEXT( "GetUpdatedFilesWithOriginStatusBranch" );
}

bool FGitWorkerGetDiffWithOriginStatusBranch::Execute( FGitSourceControlCommand & InCommand )
{
    if ( !GitSourceControlUtils::FetchRemote(
             InCommand.PathToGitBinary,
             InCommand.PathToRepositoryRoot,
             InCommand.bUsingGitLfsLocking,
             InCommand.ResultInfo.InfoMessages,
             InCommand.ResultInfo.ErrorMessages ) )
    {
        return false;
    }

    auto & operation = static_cast< FGitOperationGetDiffWithOriginStatusBranch & >( InCommand.Operation.Get() );
    TArray< FString > error_messages;

    const auto & provider = FGitSourceControlModule::GetThreadSafe()->GetProvider();
    const auto status_branches = provider.GetStatusBranchNames();
    TMap< FString, FGitSourceControlState > updated_states;

    if ( !GitSourceControlUtils::GetDiffNameStatusWithBranch(
             InCommand.PathToGitBinary,
             InCommand.PathToRepositoryRoot,
             updated_states,
             error_messages,
             status_branches[ 0 ] ) )
    {
        return false;
    }

    GitSourceControlUtils::CollectNewStates( updated_states, States );

    for ( auto & [ File, State ] : updated_states )
    {
        auto new_state = MakeShareable( new FGitSourceControlState( State ) );
        operation.AddState( new_state );
    }

    return true;
}

bool FGitWorkerGetDiffWithOriginStatusBranch::UpdateStates() const
{
    return GitSourceControlUtils::UpdateCachedStates( States );
}

#undef LOCTEXT_NAMESPACE