//Copyright 2020 Marchetti Alfredo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "Layers/Types/TerrainLayerApplicationType.h"
#include "TerrainLayerData.generated.h"

class UTerrainBaseLayoutData;

/**
 *  Defines a layer that can be applied to the level layout.
 */
UCLASS()
class TERRAINGENERATOR_API UTerrainLayerData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UTerrainLayerData();
	
	FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("TerrainLayerData", GetFName());
	}

// #if WITH_EDITORONLY_DATA
// 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AssetBundles = "Layer"), meta = (Categories = "Layer"), Category = "Layer")
// 	FLinearColor EditorLayerColor = FLinearColor::MakeRandomColor();
// #endif
		
	/* User defined data.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AssetBundles = "Layer"), meta = (Categories = "Layer"), Category = "Layer")
	UObject* LayerDataObject = nullptr;

	/* The layer tag.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AssetBundles = "Layer"), meta = (Categories = "Layer"), Category = "Layer")
	FGameplayTag LayerTag = FGameplayTag::EmptyTag;
	
	/* The layer additional tags.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AssetBundles = "Layer"), meta = (Categories = "Layer"), Category = "Layer")
	FGameplayTagContainer LayerAdditionalTags = FGameplayTagContainer::EmptyContainer;

	/* Required tags in the cell to be able to apply.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AssetBundles = "Application"), meta = (Categories = "Application"), Category = "Application")
	FGameplayTagContainer ApplicationRequiredTags= FGameplayTagContainer::EmptyContainer;
	
	/* Required tags in the cell to be able to apply.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AssetBundles = "Application"), meta = (Categories = "Application"), Category = "Application")
	FGameplayTagContainer ApplicationIgnoredTags= FGameplayTagContainer::EmptyContainer;
	
	/* Lower index will apply before. This helps for the setup of tags relationship between layers.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AssetBundles = "Application"), meta = (Categories = "Application"), Category = "Application")
	int32 ApplicationOrder = 0;

	/* Data for generating the initial cells of the layer.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AssetBundles = "Initial Layout"), meta = (Categories = "Initial Layout"), Category = "Initial Layout")
	TArray <FLayerApplicationData> InitialCellsApplicationData;
		
	/*
	 * How far the cells can go from the initial cell (the value is double, half to each side)
	 * The side size of the area around the initial cell that can be used to generate cells.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AssetBundles = "Initial Layout"), meta = (Categories = "Initial Layout"), Category = "Initial Layout")
	int32 QuadSize = 10.f;

	/* The layout of the layer.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AssetBundles = "Layout"), meta = (Categories = "Layout"), Category = "Layout")
	UTerrainBaseLayoutData* LayerLayout = nullptr;
};
