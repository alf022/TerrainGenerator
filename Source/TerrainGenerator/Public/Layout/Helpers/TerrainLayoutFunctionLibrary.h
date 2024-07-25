//Copyright 2020 Marchetti Alfredo. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Layout/Types/LayoutTypes.h"

#include "TerrainLayoutFunctionLibrary.generated.h"

UCLASS()
class TERRAINGENERATOR_API UTerrainLayoutFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:	
	UFUNCTION()
	static int32 GenerateStreamSeed(FRandomStream& Stream);
	
	UFUNCTION()
	static FIntPoint GetIDInAnchor(const TArray<FIntPoint>& IDs, FVector2D Anchor = FVector2D(.5f, .5f));

	/* Finds the cell ID closer to the provided one in the IDs pool.*/
	UFUNCTION()
	static FIntPoint GetClosesIDToID(const TArray<FIntPoint>& IDs, FIntPoint ID, int32 MaxDistance);

	/**
	*	Adyacent and diagonal cells count. 
	* 	Does not check if they are valid.
	*/
	UFUNCTION()
	static TArray<FIntPoint> GetSurroundingCellsOfCell(FIntPoint Cell, int32 Range = 1);
	
	UFUNCTION()
	static bool FindFirstCellAtRange(FIntPoint StartCell, const TArray <FIntPoint>& Cells, FIntPoint& CellAtRangeOut, int32 Range = 1);

	UFUNCTION()
	static TArray<FIntPoint> GetCellsOfCellAtRange(FIntPoint Cell, int32 Range = 1);

	/**
	*	Get the adyacents IDs of a cell.
	*	Does not check if they are valid.
	*	Diagonal cells dont count.
	*/
	UFUNCTION()
	static TArray<FIntPoint> GetAdyacentCellsOfCell(FIntPoint CellIn);

	/**
	*	Get the cell in the direction specified. 
	*	Directions start up and goes clockwise.
	*	0 is Up. 1 is Right. 2 is Down. 3 is Left.
	*/
	UFUNCTION()
	static FIntPoint GetAdyacentCellInDirectionOfCell(const ETerrainGen_CorridorDirection& DirectionIn, const FIntPoint& CellIn);
		
	/*Gets the cell ID(matrix element) that holds the specific world position.*/
	UFUNCTION()
	static FIntPoint GetCellIDInWorldLocation(FVector WorldLocationIn, float CellSizeIn, FVector InitialLocationIn);

	/*Gets the world position of the cell.*/
	UFUNCTION()
	static FVector GetCellWorldPosition(FIntPoint CellsIDIn, float CellSizeIn, FVector2D AnchorIn = FVector2D(0.5f, 0.5f));

	/*Gets the cells around a cell in an area*/
	UFUNCTION()
	static TArray<FIntPoint> GetCellsInArea(const TMap<FIntPoint, FCellLayout>& CellsDataIn, float CellSizeIn, FVector InitialLocationIn, FVector WorldLocationIn, int32 CellRadious);

	/*Gets the world position distance between two cells.*/
	UFUNCTION()
	static float GetWorldDistanceBetweenCells(FIntPoint CellA, FIntPoint CellB, float CellSizeIn);
	
	/**
	*	Fin the max cell ID on x and y.
	*/
	UFUNCTION()
	static FIntPoint GetMaxGridSize(const TArray<FIntPoint>& Cells);

	UFUNCTION()
	static FIntPoint GetMinGridSize(const TArray<FIntPoint>& Cells);

	UFUNCTION()
	static float GetCellDistanceBetweenCells(FIntPoint CellA, FIntPoint CellB);

	UFUNCTION()
	static float GetMinimunCellDistanceBetweenCells(const TArray<FIntPoint>& CellsA, const TArray<FIntPoint>& CellsB);

	UFUNCTION()
	static float GetRoomVectorAngle(FIntPoint RoomCentralCell, FIntPoint InitialCell);
};
