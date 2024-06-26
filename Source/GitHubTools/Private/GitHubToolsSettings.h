#pragma once

#include <CoreMinimal.h>
#include <Engine/DeveloperSettings.h>

#include "GitHubToolsSettings.generated.h"

UCLASS( config = EditorPerProjectUserSettings, MinimalAPI, meta = ( DisplayName = "GitHub Tools" ) )
class UGitHubToolsSettings final : public UDeveloperSettings
{
    GENERATED_BODY()

public:
    UGitHubToolsSettings();

    UPROPERTY( Config, EditDefaultsOnly )
    FString RepositoryOwner;

    UPROPERTY( Config, EditDefaultsOnly )
    FString RepositoryName;

    UPROPERTY( Config, EditDefaultsOnly )
    FString Token;

    UPROPERTY( Config, EditDefaultsOnly )
    uint8 bMarkFileViewedAutomatically : 1;
};
