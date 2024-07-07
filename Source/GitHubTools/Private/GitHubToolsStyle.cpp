// Copyright Epic Games, Inc. All Rights Reserved.

#include "GitHubToolsStyle.h"

#include "Framework/Application/SlateApplication.h"
#include "GitHubTools.h"
#include "Interfaces/IPluginManager.h"
#include "Slate/SlateGameResources.h"
#include "Styling/SlateStyleMacros.h"
#include "Styling/SlateStyleRegistry.h"

#define RootToContentDir Style->RootToContentDir

TSharedPtr< FSlateStyleSet > FGitHubToolsStyle::StyleInstance = nullptr;

void FGitHubToolsStyle::Initialize()
{
    if ( !StyleInstance.IsValid() )
    {
        StyleInstance = Create();
        FSlateStyleRegistry::RegisterSlateStyle( *StyleInstance );
    }
}

void FGitHubToolsStyle::Shutdown()
{
    FSlateStyleRegistry::UnRegisterSlateStyle( *StyleInstance );
    ensure( StyleInstance.IsUnique() );
    StyleInstance.Reset();
}

FName FGitHubToolsStyle::GetStyleSetName()
{
    static FName StyleSetName( TEXT( "GitHubToolsStyle" ) );
    return StyleSetName;
}

const FVector2D Icon16x16( 16.0f, 16.0f );
const FVector2D Icon20x20( 20.0f, 20.0f );

TSharedRef< FSlateStyleSet > FGitHubToolsStyle::Create()
{
    TSharedRef< FSlateStyleSet > Style = MakeShareable( new FSlateStyleSet( "GitHubToolsStyle" ) );
    Style->SetContentRoot( IPluginManager::Get().FindPlugin( "GitHubTools" )->GetBaseDir() / TEXT( "Resources" ) );

    Style->Set( "GitHubTools.PluginAction", new IMAGE_BRUSH_SVG( TEXT( "PlaceholderButtonIcon" ), Icon20x20 ) );
    return Style;
}

void FGitHubToolsStyle::ReloadTextures()
{
    if ( FSlateApplication::IsInitialized() )
    {
        FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
    }
}

const ISlateStyle & FGitHubToolsStyle::Get()
{
    return *StyleInstance;
}
