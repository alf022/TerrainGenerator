//Copyright 2020 Marchetti Alfredo. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Layout/Types/LayoutTypes.h"
#include "TerrainLayoutLayerData.generated.h"

class UBaseLayoutObject;
class UTerrainLayoutSubsystem;

UCLASS()
class TERRAINGENERATOR_API UTerrainLayoutLayerData : public UPrimaryDataAsset
{
	GENERATED_BODY()
				
public:
	FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("TerrainLayoutLayerData", GetFName());
	}
	
	/* How many times this layer has to be applied.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AssetBundles = "Layout"), meta = (Categories = "Layout"), Category = "Layout")
	int32 LayerApplicationTimes = 1;

	/* The objects that can be used for Layout layouts.*/
	UPROPERTY(Instanced, EditAnywhere, BlueprintReadOnly, meta = (AssetBundles = "Layout"), meta = (Categories = "Layout"), Category = "Layout")
	TArray <UBaseLayoutObject*> LayoutObjects;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AssetBundles = "Layout"), meta = (Categories = "Layout"), Category = "Layout")
	int32 LayoutMinCells = 5;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AssetBundles = "Layout"), meta = (Categories = "Layout"), Category = "Layout")
	int32 LayoutMaxCells = 10;

	void ApplyLayer(FRandomStream& Stream, FLayoutInputData& LayoutInputData);

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;	
#endif

};
