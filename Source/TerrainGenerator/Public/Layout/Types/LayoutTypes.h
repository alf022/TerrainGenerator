//Copyright 2020 Marchetti Alfredo. All Rights Reserved.
#pragma once

#include "GameplayTagContainer.h"
#include "LayoutTypes.generated.h"

USTRUCT(BlueprintType)
struct FLayoutInputData
{
	GENERATED_USTRUCT_BODY()

public:
	FLayoutInputData()
	{
		Cells = TArray<FIntPoint>();
		CellsBounds = TArray<FIntPoint>();
		Bounds_Min_X = 0;
		Bounds_Max_X = 0;
		Bounds_Min_Y = 0;
		Bounds_Max_Y = 0;
		MinCells = 0;
		MaxCells = 0;
		LayerIndex = 0;
		LayerApplicationIndex = 0;	
		ToGenerateCellsScale = 1.f;
	}

	UPROPERTY()
	TArray<FIntPoint> Cells;
	
	/* Extra cells to check for bounds. The layout must be contained in these cells, unless is empty.*/
	UPROPERTY()
	TArray<FIntPoint> CellsBounds;

	UPROPERTY()
	int32 Bounds_Min_X;

	UPROPERTY()
	int32 Bounds_Max_X;

	UPROPERTY()
	int32 Bounds_Min_Y;

	UPROPERTY()
	int32 Bounds_Max_Y;
	
	UPROPERTY()
	int32 MinCells;

	UPROPERTY()
	int32 MaxCells;

	UPROPERTY()
	int32 LayerIndex;

	UPROPERTY()
	int32 LayerApplicationIndex;

	UPROPERTY()
	float ToGenerateCellsScale;
};

USTRUCT()
struct FCellLayout
{
	GENERATED_USTRUCT_BODY()

public:
	FCellLayout()
	{
		CellID = { -1,-1 };
		RoomID = { -1,-1 };
		LayoutDepth = 0;
		Tags = FGameplayTagContainer::EmptyContainer;
	}

	UPROPERTY()
	FIntPoint CellID;

	UPROPERTY()
	FIntPoint RoomID;

	/*The minimun distance of the cell to the terrain border, in cells.*/
	UPROPERTY()
	int32 LayoutDepth;

	UPROPERTY()
	FGameplayTagContainer Tags;
};

USTRUCT()
struct FRoomLayout
{
	GENERATED_USTRUCT_BODY()

public:
	FRoomLayout()
	{
		InitialCell = { -1,-1 };
		CentralCell = { -1,-1 };
		DungeonDepth = 0;
		Cells = TArray <FIntPoint>();
	}

	UPROPERTY()
	FIntPoint InitialCell;

	UPROPERTY()
	FIntPoint CentralCell;

	/*Distance to start point in "rooms".*/
	UPROPERTY()
	int32 DungeonDepth;

	UPROPERTY()
	TArray <FIntPoint> Cells;
};

USTRUCT()
struct FIntPointPair
{
	GENERATED_USTRUCT_BODY()

public:
	FIntPointPair()
	{
		PointA = { -1,-1 };
		PointB = { -1,-1 };
	}

	UPROPERTY()
	FIntPoint PointA;

	UPROPERTY()
	FIntPoint PointB;
};
