#pragma once

#include <CoreMinimal.h>
#include <Engine/DeveloperSettings.h>

#include "GitHubToolsSettings.generated.h"

UCLASS( config = EditorPerProjectUserSettings, MinimalAPI, meta = ( DisplayName = "GitHub Tools" ) )
class UGitHubToolsSettings final : public UDeveloperSettings
{
    GENERATED_BODY()

public:
    UPROPERTY( Config, EditDefaultsOnly )
    FString Token;
};
