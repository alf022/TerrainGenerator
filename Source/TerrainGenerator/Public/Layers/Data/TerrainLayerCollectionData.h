//Copyright 2020 Marchetti Alfredo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TerrainLayerCollectionData.generated.h"

class UTerrainLayerData;

/**
 *	Configuration of layers added to the terrain layout.
 */
UCLASS()
class TERRAINGENERATOR_API UTerrainLayerCollectionData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AssetBundles = "Layer"), meta = (Categories = "Layer"), Category = "Layer")
	TArray<UTerrainLayerData*> TerrainLayers;
};
