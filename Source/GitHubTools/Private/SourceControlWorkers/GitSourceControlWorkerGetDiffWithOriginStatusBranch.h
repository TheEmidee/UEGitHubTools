#pragma once

#include "GitSourceControlState.h"
#include "IGitSourceControlWorker.h"

#include <CoreMinimal.h>

class FGitWorkerGetDiffWithOriginStatusBranch final : public IGitSourceControlWorker
{
public:
    virtual ~FGitWorkerGetDiffWithOriginStatusBranch() = default;
    virtual FName GetName() const override;
    virtual bool Execute( FGitSourceControlCommand & InCommand ) override;
    virtual bool UpdateStates() const override;

    TMap< const FString, FGitState > States;
};
