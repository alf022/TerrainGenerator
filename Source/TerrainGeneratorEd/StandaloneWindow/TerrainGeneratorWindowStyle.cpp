//Copyright 2020 Marchetti Alfredo. All Rights Reserved.

#include "TerrainGeneratorWindowStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "Framework/Application/SlateApplication.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleMacros.h"

#define RootToContentDir Style->RootToContentDir

TSharedPtr<FSlateStyleSet> FTerrainGeneratorWindowStyle::StyleInstance = nullptr;

void FTerrainGeneratorWindowStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FTerrainGeneratorWindowStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FTerrainGeneratorWindowStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("TerrainGeneratorWindowStyle"));
	return StyleSetName;
}

const FVector2D WindowIcon16x16(16.0f, 16.0f);
const FVector2D WindowIcon20x20(20.0f, 20.0f);

TSharedRef< FSlateStyleSet > FTerrainGeneratorWindowStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("TerrainGeneratorWindowStyle"));

	Style->SetContentRoot(IPluginManager::Get().FindPlugin("TerrainGenerator")->GetBaseDir() / TEXT("Resources"));
	Style->Set("TerrainGeneratorWindow.OpenPluginWindow", new IMAGE_BRUSH(TEXT("S_Terrain"), WindowIcon20x20));
	
	/*Style->SetContentRoot(FPaths::EngineContentDir() / TEXT("EditorResources"));
	Style->Set("TerrainGeneratorWindow.OpenPluginWindow", new IMAGE_BRUSH(TEXT("S_Terrain"), WindowIcon20x20));*/
	return Style;
}

void FTerrainGeneratorWindowStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FTerrainGeneratorWindowStyle::Get()
{
	return *StyleInstance;
}
