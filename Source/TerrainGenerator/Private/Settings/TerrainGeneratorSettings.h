//Copyright 2020 Marchetti Alfredo. All Rights Reserved.

#pragma once

#include "Engine/DeveloperSettingsBackedByCVars.h"
#include "HAL/Platform.h"

#include "TerrainGeneratorSettings.generated.h"

/**
 * Settings for terrain generation
 */
UCLASS(config=Game, defaultconfig, meta=(DisplayName="Terrain Generator"))
class UTerrainGeneratorSettings : public UDeveloperSettingsBackedByCVars
{
	GENERATED_BODY()

public:
	UTerrainGeneratorSettings();

	//@TODO: This settings are not getting used, since this part is not developed yet.

	/*The distance between two consecutive vertex*/
 	UPROPERTY(config, EditAnywhere, Category=Configuration)
	float VertexDistance = 50.0f;

	/*Used for calculating UVs of the vertex*/
	UPROPERTY(config, EditAnywhere, Category = Configuration)
	float VertexSize = 1.0f;

	UPROPERTY(config, EditAnywhere, Category = Configuration)
	int32 VertexMax = 50;
};

