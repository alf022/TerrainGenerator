//Copyright 2020 Marchetti Alfredo. All Rights Reserved.

#pragma once

#include "Engine/DeveloperSettingsBackedByCVars.h"
#include "HAL/Platform.h"
#include "Terrain/Data/TerrainData.h"

#include "TerrainGeneratorEditorSettings.generated.h"

/**
 * Settings for a terrain generation in editor
 */
UCLASS(config=Game, defaultconfig, meta=(DisplayName="Terrain Generator Editor Settings"))
class UTerrainGeneratorEditorSettings : public UDeveloperSettingsBackedByCVars
{
	GENERATED_BODY()

public:
	UTerrainGeneratorEditorSettings();

public:
	/* Override for data in editor.*/
	UPROPERTY(config, EditAnywhere, Category=Configuration)
	TSoftObjectPtr<UTerrainData> EditorTerrainData = nullptr;	
};

