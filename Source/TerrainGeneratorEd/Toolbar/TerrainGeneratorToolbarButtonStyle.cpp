//Copyright 2020 Marchetti Alfredo. All Rights Reserved.

#include "TerrainGeneratorToolbarButtonStyle.h"
#include "TerrainGeneratorToolbarButtonCommands.h"
#include "Framework/Application/SlateApplication.h"
#include "Styling/SlateStyleRegistry.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleMacros.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

#define RootToContentDir Style->RootToContentDir

TSharedPtr<FSlateStyleSet> FTerrainGeneratorToolbarButtonStyle::StyleInstance = nullptr;

void FTerrainGeneratorToolbarButtonStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FTerrainGeneratorToolbarButtonStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FTerrainGeneratorToolbarButtonStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("TerrainGeneratorToolbarButtonStyle"));
	return StyleSetName;
}

const FVector2D ToolbarIcon16x16(16.0f, 16.0f);
const FVector2D ToolbarIcon20x20(50.0f, 50.0f);

TSharedRef< FSlateStyleSet > FTerrainGeneratorToolbarButtonStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("TerrainGeneratorToolbarButtonStyle"));			
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("TerrainGenerator")->GetBaseDir() / TEXT("Resources"));
	Style->Set("TerrainGeneratorToolbarButton.PluginAction", new IMAGE_BRUSH(TEXT("S_Terrain"), ToolbarIcon20x20));
	return Style;
}

void FTerrainGeneratorToolbarButtonStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FTerrainGeneratorToolbarButtonStyle::Get()
{
	return *StyleInstance;
}
