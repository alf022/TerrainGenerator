//Copyright 2020 Marchetti Alfredo. All Rights Reserved.
#include "Layout/LayoutObjects/LayoutObjectRandomUnconnected.h"
#include "Logs/TerrainGeneratorLogs.h"

void ULayoutObjectRandomUnconnected::GenerateLayout_Implementation(int32 SeedIn, FLayoutInputData& InLayoutInputData)
{
	if (InLayoutInputData.Cells.Num() <= 0)
	{
		return;
	}

	Super::GenerateLayout_Implementation(SeedIn, InLayoutInputData);

	TArray<FIntPoint> GeneratedCells = InLayoutInputData.Cells;
	TArray<FIntPoint> PoolCells = TArray<FIntPoint>();

	//Generate pool
	if (bIgnoreQuadBounds)
	{
		PoolCells = InLayoutInputData.CellsBounds;
	}
	else
	{
		for (int32 i = InLayoutInputData.Bounds_Min_X; i <= InLayoutInputData.Bounds_Max_X; i++)
		{
			for (int32 j = InLayoutInputData.Bounds_Min_Y; j <= InLayoutInputData.Bounds_Max_Y; j++)
			{
				if (IsCellInBounds(FIntPoint(i, j), InLayoutInputData))
				{
					PoolCells.Add(FIntPoint(i, j));
				}
			}
		}
	}	

	for (int32 i = 0; i < ToGenerateCellsAmount; i++)
	{
		int32 SelectedPoolIndex = Stream.RandRange(0, PoolCells.Num() - 1);

		if (!PoolCells.IsValidIndex(SelectedPoolIndex))
		{
			UE_LOG(TerrainGeneratorLog, Error, TEXT("ULayoutObjectRandomUnconnected::GenerateLayout_Implementation - Invalid index!."));
			break;
		}

		GeneratedCells.AddUnique(PoolCells[SelectedPoolIndex]);
		PoolCells.RemoveAtSwap(SelectedPoolIndex);

		if (PoolCells.Num() <= 0)
		{
			UE_LOG(TerrainGeneratorLog, Error, TEXT("ULayoutObjectRandomUnconnected::GenerateLayout_Implementation - Generation stopped since there is no more room in the quad!."));
			break;
		}
	}

	for (const FIntPoint& Cell : GeneratedCells)
	{
		InLayoutInputData.Cells.AddUnique(Cell);
	}
}
