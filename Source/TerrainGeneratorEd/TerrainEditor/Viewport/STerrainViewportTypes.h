//Copyright 2020 Marchetti Alfredo. All Rights Reserved.

#pragma once

#include "STerrainViewportTypes.generated.h"

USTRUCT()
struct FCellGridData
{
	GENERATED_USTRUCT_BODY()

public:
	FCellGridData()
	{
		GridID = { 0,0 };
		Color = { .2f, .2f , .2f };
	}

	UPROPERTY()
	FIntPoint GridID;

	UPROPERTY()
	FLinearColor Color;
};

USTRUCT()
struct FViewportConfiguration
{
	GENERATED_USTRUCT_BODY()

public:
	FViewportConfiguration()
	{
		bUseGridMesh = true;
		bUseGrid = true;
		bDrawInitialCell = false;
		bDrawCentralCell = true;
		bDrawInitialRoom = true;
		bDrawRoomBorders = false;
		bDrawRoomCollision = true;
		bDrawCorridors = true;
		bDrawWalls = true;
		bDrawDoors = false;
		bDrawCells = true;
	}

	UPROPERTY()
	bool bUseGridMesh = true;
	UPROPERTY()
	bool bUseGrid = true;

	UPROPERTY()
	bool bDrawInitialCell = false;
	UPROPERTY()
	bool bDrawCentralCell = true;
	UPROPERTY()
	bool bDrawInitialRoom = true;
	UPROPERTY()
	bool bDrawRoomBorders = false;
	UPROPERTY()
	bool bDrawRoomCollision = true;
	UPROPERTY()
	bool bDrawCorridors = true;
	UPROPERTY()
	bool bDrawWalls = true;
	UPROPERTY()
	bool bDrawDoors = false;
	UPROPERTY()
	bool bDrawCells = true;
};