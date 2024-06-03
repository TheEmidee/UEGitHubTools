// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GitSourceControlModule.h"
#include "GitSourceControlState.h"
#include "SourceControlAssetDataCache.h"

#include <CoreMinimal.h>

struct FGitSourceControlReviewFileItem final : TSharedFromThis< FGitSourceControlReviewFileItem >
{
    explicit FGitSourceControlReviewFileItem( const TSharedRef< FGitSourceControlState > & file_state );
    ~FGitSourceControlReviewFileItem() = default;

	FText GetFileName() const
    {
        return FText::FromString( FileState->GetFilename() );
    }

	/** Returns the asset name of the item. This might update the asset names from the asset registry. */
    FText GetAssetName()
	{
        return AssetName;
	}

    /** Returns the package name of the item to display */
    FText GetPackageName() const
    {
        return PackageName;
    }

	/** Returns the checkbox state of this item */
    ECheckBoxState GetCheckBoxState() const
    {
        return CheckBoxState;
    }

    /** Sets the checkbox state of this item */
    void SetCheckBoxState( ECheckBoxState NewState )
    {
        CheckBoxState = NewState;
    }

	FName GetIconName() const
	{
        return FileState->GetIcon().GetStyleName();
	}

	FText GetIconTooltip() const
    {
        return FileState->GetDisplayTooltip();
    }

	void RefreshAssetInformation();

	/** Shared pointer to the source control state object itself */
    TSharedRef< FGitSourceControlState > FileState;

	/** Matching asset(s) to facilitate Locate in content browser */
    FAssetDataArrayPtr Assets;

	FText AssetName;
    FString AssetNameStr;

    /** Cached asset path to display */
    FText AssetPath;
    FString AssetPathStr;

    /** Cached asset type to display */
    FText AssetType;
    FString AssetTypeStr;

	/** Cached package name to display */
    FText PackageName;

	/** Cached asset type related color to display */
    FColor AssetTypeColor;
    ECheckBoxState CheckBoxState;
};