//Copyright 2020 Marchetti Alfredo. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "TerrainLayerLayout.generated.h"

class UTerrainLayerData;

USTRUCT()
struct FTerrainLayerLayout
{
	GENERATED_USTRUCT_BODY()

public:
	FTerrainLayerLayout()
	{
		LayerData = nullptr;
		InitialCell = FIntPoint();
		CellsGenerated = TArray<FIntPoint>();
	}

	UPROPERTY()
	UTerrainLayerData* LayerData= nullptr;

	UPROPERTY()
	FIntPoint InitialCell = FIntPoint();
	
	UPROPERTY()
	TArray<FIntPoint> CellsGenerated= TArray<FIntPoint>();
};
