//Copyright 2020 Marchetti Alfredo. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include "Layout/Data/TerrainBaseLayoutData.h"
#include "Layout/Types/CorridorTypes.h"
#include "TerrainLayoutData.generated.h"

class UTerrainLayoutLayerData;
class UTerrainLayoutRoomData;

UCLASS()
class TERRAINGENERATOR_API UTerrainLayoutData : public UPrimaryDataAsset
{
	GENERATED_BODY()
				
public:
	FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("TerrainLayoutData", GetFName());
	}
		
	/* How the quads are placed for rooms.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0"), meta = (AssetBundles = "Initial Layout"), meta = (Categories = "Initial Layout"), Category = "Initial Layout")
	UTerrainBaseLayoutData* InitialRoomsLayout;

	/**
	*	How many cells are used to calculate the actual room collision on layout. Lower values can improve heavily processing time.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.05", ClampMax = "1"), meta = (AssetBundles = "Rooms Separation"), meta = (Categories = "Rooms Separation"), Category = "Rooms Separation")
	float RoomSeparationPrecision = .5f;
	
	/* This will create a pattern trying to reduce the number of cells used in calculation for separation, based on oddity.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "1"), meta = (AssetBundles = "Rooms Separation"), meta = (Categories = "Rooms Separation"), Category = "Rooms Separation")
	int32 RoomSeparationPatternIndex = 2;

	/*The min distance  between each room.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AssetBundles = "Rooms Separation"), meta = (Categories = "Rooms Separation"), Category = "Rooms Separation")
	int32 MinRoomDistance = 3;

	/*The max distance  between each room.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AssetBundles = "Rooms Separation"), meta = (Categories = "Rooms Separation"), Category = "Rooms Separation")
	int32 MaxRoomDistance = 8;

	/* The minimun width of the corridors.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AssetBundles = "Corridors Layout"), meta = (Categories = "Corridors Layout"), Category = "Corridors Layout")
	int32 CorridorsMinRange = 1;

	/* The Maximun width of the corridors.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AssetBundles = "Corridors Layout"), meta = (Categories = "Corridors Layout"), Category = "Corridors Layout")
	int32 CorridorsMaxRange = 2;

	/** The percentage of circular corridors compared to rooms  More than 1 will generate double corridors or more per room.*/
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0"), Category = "Corridors Layout")
	float CircularCorridorsAmountPercent = .5f;

	/** 
	*	The max distance that separates the two rooms of circular corridor in cells.
	*	Negative values means that any corridor is eligible.
	*/
	UPROPERTY(EditAnywhere, Category = "Corridors Layout")
	int32 MaxCircularCorridorsCellDistance = -1;

	/**
	*	The multiplier to max room separation to add for corridor detection. 
	*	A value of 1 means it will get rooms near by the start room. A value of 2 means it will try to get aproximately 2 layers of rooms surronding start, etc.
	*	Smaller values improves performance.
	*/
	UPROPERTY(EditAnywhere, meta = (ClampMin = "1"), Category = "Corridors Layout")
	float CorridorDetectionMultiplier = 1.f;

	/** How the corridors should be selected.*/
	UPROPERTY(EditAnywhere, Category = "Corridors Layout")
	ETerrainGen_CorridorPathSelection CorridorSelectionType = ETerrainGen_CorridorPathSelection::Threshold;

	/**
	*	The threshold to use when using that Corridor selection type.
	*	This threshold is added to the minimun possible distance.
	*	If 0, will generate the shortest possible corridors.
	*/
	UPROPERTY(EditAnywhere, Category = "Corridors Layout")
	int32 CorridorSelectionThreshold = 5;

	/* The different room layouts.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AssetBundles = "Rooms Layout"), meta = (Categories = "Rooms Layout"), Category = "Rooms Layout")
	TArray <UTerrainLayoutRoomData*> RoomLayouts;
	
	/* How many cells of wall should be placed.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AssetBundles = "Walls Layout"), meta = (Categories = "Walls Layout"), Category = "Walls Layout")
	int32 WallsRange = 3;

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif

};
