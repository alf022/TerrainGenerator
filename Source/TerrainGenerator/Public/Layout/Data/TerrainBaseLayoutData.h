//Copyright 2020 Marchetti Alfredo. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TerrainBaseLayoutData.generated.h"

class UTerrainLayoutLayerData;

UCLASS()
class TERRAINGENERATOR_API UTerrainBaseLayoutData : public UPrimaryDataAsset
{
	GENERATED_BODY()
				
public:
	FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("TerrainBaseLayoutData", GetFName());
	}
		
	/* If the starting cell should be the central cell of the quad or random.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AssetBundles = "Initial Cell"), meta = (Categories = "Initial Cell"), Category = "Initial Cell")
	bool bRandomizeInitialCell = false;

	/**
	*	How much deviation from center is allowed for the initial cell in percent to be randomized in if bRandomizeInitialCell = true.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0", ClampMax = "1"), meta = (AssetBundles = "Initial Cell"), meta = (Categories = "Initial Cell"), Category = "Initial Cell")
	float InitialCellMaxDeviationFromCenterPercent = 1.f;

	/* Initial layout position relative to the grid for the generation to start when using as initial layout for placing the initial cell if bRandomizeInitialCell = false.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AssetBundles = "Initial Cell"), meta = (Categories = "Initial Cell"), Category = "Initial Cell")
	FVector2D InitialCellAnchor = FVector2D(.5, .5f);

	/**
	*	The quad size determines the space that is available to generate cells. 
	*	It gives more wiggle room to generate more random patterns when is bigger, but makes calculations more costly.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0"), meta = (AssetBundles = "Layout"), meta = (Categories = "Layout"), Category = "Layout")
	float ExtraQuadSizePercent = 1.f;

	/* The layout layers to apply for this Base.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AssetBundles = "Layout"), meta = (Categories = "Layout"), Category = "Layout")
	TArray <UTerrainLayoutLayerData*> LayoutLayers;

	int32 GetMaxCells() const;
	int32 GetQuadSize() const;

	/* *
	*	Generates an array of points.
	*	QuadSize is the Amount of cells per side of the initial grid. QuadSize = 4 will use an 4x4 grid to generate layout.
	*/
	TArray <FIntPoint> GenerateLayout(FIntPoint& InitialCellOut, FRandomStream& Stream, FIntPoint LayoutOffset = FIntPoint(), const TArray<FIntPoint>& CellBounds = TArray<FIntPoint>()) const;
	
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};
