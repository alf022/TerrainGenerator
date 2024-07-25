//Copyright 2020 Marchetti Alfredo. All Rights Reserved.
#pragma once

#include "CorridorTypes.generated.h"

UENUM(BlueprintType)
enum class ETerrainGen_CorridorPathSelection : uint8
{
	Random,
	Threshold,
};

UENUM()
enum class ETerrainGen_CorridorDirection : uint8
{
	Up,
	Down,
	Left,
	Right,
};

UENUM()
enum class EPathFindingNodeState : uint8
{
	None,
	Open,
	Closed,
};

/*Base struct for defining distance between rooms layouts.*/
USTRUCT()
struct FTerrain_RoomDistance
{
	GENERATED_USTRUCT_BODY()

public:

	FTerrain_RoomDistance()
	{
		Distance = 0;
		RoomA = 0;
		RoomB = 0;
	}

	/*The distance in the world.*/
	UPROPERTY()
	float Distance;

	/*Room A*/
	UPROPERTY()
	FIntPoint RoomA;
	
	/*Room B*/
	UPROPERTY()
	FIntPoint RoomB;

	FORCEINLINE bool operator==(const FTerrain_RoomDistance& Other) const
	{
		return (RoomA == Other.RoomA && RoomB == Other.RoomB) || (RoomA == Other.RoomB && RoomB == Other.RoomA);
	}

	FORCEINLINE bool operator!=(const FTerrain_RoomDistance& Other) const
	{
		return !((RoomA == Other.RoomA && RoomB == Other.RoomB) || (RoomA == Other.RoomB && RoomB == Other.RoomA));
	}
};

USTRUCT()
struct FTerrain_CellDistance
{
	GENERATED_USTRUCT_BODY()

public:

	FTerrain_CellDistance()
	{
		UnitCellDistance = 0;
		CellA = { 0,0 };
		CellB = { 0,0 };
	}

	/*The distance in cells.*/
	UPROPERTY()
	int32 UnitCellDistance;

	/*Cell A*/
	UPROPERTY()
	FIntPoint CellA;

	/*Cell B*/
	UPROPERTY()
	FIntPoint CellB;

	FORCEINLINE bool operator==(const FTerrain_CellDistance& Other) const
	{
		return (CellA == Other.CellA && CellB == Other.CellB) || (CellA == Other.CellB && CellB == Other.CellA);
	}

	FORCEINLINE bool operator!=(const FTerrain_CellDistance& Other) const
	{
		return !((CellA == Other.CellA && CellB == Other.CellB) || (CellA == Other.CellB && CellB == Other.CellA));
	}
};

/*Base struct that contains the Terrain layout data.*/
USTRUCT()
struct FPathFindingCellData
{
	GENERATED_USTRUCT_BODY()

public:

	FPathFindingCellData()
	{
		Cell_ID = FIntPoint();
		Parent = FIntPoint();
		State = EPathFindingNodeState::None;
		GCost = 0;
		HCost = 0;
	}

	/**
	*	The Terrain(map) asset name to create.
	*/
	UPROPERTY()
	FIntPoint Cell_ID;

	/*The parent node of this node.*/
	UPROPERTY()
	FIntPoint Parent;

	/*The node state.*/
	UPROPERTY()
	EPathFindingNodeState State;

	/*The distance to the start node. G cost */
	UPROPERTY()
	float GCost;

	/*The distance to the end node. H cost */
	UPROPERTY()
	float HCost;

	/*Returns the F cost.*/
	FORCEINLINE int32 GetFCost() const
	{
		return GCost + HCost;
	}

	FORCEINLINE bool operator==(const FPathFindingCellData& Other) const
	{
		return (Cell_ID == Other.Cell_ID);
	}

	FORCEINLINE bool operator!=(const FPathFindingCellData& Other) const
	{
		return (Cell_ID != Other.Cell_ID);
	}
};

USTRUCT()
struct FCorridorLayout
{
	GENERATED_USTRUCT_BODY()

public:
	FCorridorLayout()
	{
		StartRoomId = FIntPoint();
		EndRoomId = FIntPoint();
		StartCellId = FIntPoint();
		EndCellId = FIntPoint();
		Cells = TArray<FIntPoint>();
	}

	UPROPERTY()
	FIntPoint StartRoomId;

	UPROPERTY()
	FIntPoint EndRoomId;

	UPROPERTY()
	TArray<FIntPoint> Cells;

	UPROPERTY()
	FIntPoint StartCellId;

	UPROPERTY()
	FIntPoint EndCellId;
};

