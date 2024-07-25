//Copyright 2020 Marchetti Alfredo. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TerrainData.generated.h"

class UTerrainLayoutData;
class UTerrainData;
class UTerrainLayerCollectionData;

UCLASS()
class TERRAINGENERATOR_API UTerrainData : public UPrimaryDataAsset
{
	GENERATED_BODY()
				
public:
	FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("TerrainData", GetFName());
	}

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AssetBundles = "Editor"), meta = (Categories = "Editor"), Category = "Editor")
	bool bGenerateTerrainActorInEditor = true;
#endif

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AssetBundles = "Seed"), meta = (Categories = "Seed"), Category = "Seed")
	bool bRandomSeed = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AssetBundles = "Seed"), meta = (Categories = "Seed"), Category = "Seed")
	int32 Seed = 0;

	/**
	*	The maximum amount of threads to create.
	*/
	UPROPERTY(EditAnywhere, Category = "Multi Thread")
	int32 MaxThreads = 16;

	/*The size of a cell in unreal units.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "1"), meta = (AssetBundles = "Cells"), meta = (Categories = "Cells"), Category = "Cells")
	float CellSize = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AssetBundles = "Terrain Layout"), meta = (Categories = "Terrain Layout"), Category = "Terrain Layout")
	UTerrainLayoutData* TerrainLayoutData;

	/* The layers collections applied to the layout.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AssetBundles = "Terrain Layers"), meta = (Categories = "Terrain Layers"), Category = "Terrain Layers")
	UTerrainLayerCollectionData* TerrainLayersCollection;

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};
