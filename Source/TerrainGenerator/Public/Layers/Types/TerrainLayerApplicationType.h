//Copyright 2020 Marchetti Alfredo. All Rights Reserved.
#pragma once

#include "TerrainLayerApplicationType.generated.h"


UENUM(BlueprintType)
enum class ETerrainLayerApplicationType : uint8
{
	Cell,
	Room,
	InitialRoom,	
	Corridor,	
};

UENUM(BlueprintType)
enum class ETerrainLayerApplicationMode : uint8
{
	Any,
	PerEach,
};

USTRUCT(BlueprintType)
struct FLayerApplicationData
{
	GENERATED_USTRUCT_BODY()

public:
	FLayerApplicationData()
	{
		MinLayersAmount = 1;
		MaxLayersAmount = 3;
		LayerApplicationChance = .3f;
		Mode = ETerrainLayerApplicationMode::Any;
		Type = ETerrainLayerApplicationType::Cell;
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AssetBundles = "Initial Layout"), meta = (Categories = "Initial Layout"), Category = "Initial Layout")
	ETerrainLayerApplicationMode Mode = ETerrainLayerApplicationMode::Any;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AssetBundles = "Initial Layout"), meta = (Categories = "Initial Layout"), Category = "Initial Layout")
	ETerrainLayerApplicationType Type = ETerrainLayerApplicationType::Cell;

	/* Minimun layers amount per aplication type.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 1) ,meta = (AssetBundles = "Initial Layout"), meta = (Categories = "Initial Layout"), Category = "Initial Layout")
	int32 MinLayersAmount = 1;

	/* Layers amount per aplication type.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 1), meta = (AssetBundles = "Initial Layout"), meta = (Categories = "Initial Layout"), Category = "Initial Layout")
	int32 MaxLayersAmount = 3;

	/* Rolled each time we try to apply a layer after the minimun layers amount have been applied.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 0.f), meta = (ClampMax = 1.f), Category = "Initial Layout")
	float LayerApplicationChance = .3f;
};
