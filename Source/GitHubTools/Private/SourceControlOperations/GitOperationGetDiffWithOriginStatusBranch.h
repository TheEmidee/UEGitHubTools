#pragma once

#include "CoreMinimal.h"
#include "GitSourceControlState.h"
#include "SourceControlOperationBase.h"

class FGitOperationGetDiffWithOriginStatusBranch : public FSourceControlOperationBase
{
public:
    FName GetName() const override;
    FText GetInProgressString() const override;
    void SetDescription( const FText & description );
    const FText & GetDescription() const;
    void SetSuccessMessage( const FText & success_message );
    const FText & GetSuccessMessage() const;
    void AddState( TSharedRef< FGitSourceControlState > state );
    const TArray< TSharedRef< FGitSourceControlState > > & GetFiles() const;

protected:
    TArray< TSharedRef< FGitSourceControlState > > Files;
    FText Description;
    FText SuccessMessage;
};