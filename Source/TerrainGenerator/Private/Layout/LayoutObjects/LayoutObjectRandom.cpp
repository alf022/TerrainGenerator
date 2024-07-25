//Copyright 2020 Marchetti Alfredo. All Rights Reserved.
#include "Layout/LayoutObjects/LayoutObjectRandom.h"
#include "Logs/TerrainGeneratorLogs.h"

void ULayoutObjectRandom::GenerateLayout_Implementation(int32 SeedIn, FLayoutInputData& InLayoutInputData)
{
	if (InLayoutInputData.Cells.Num() <= 0)
	{
		return;
	}

	Super::GenerateLayout_Implementation(SeedIn, InLayoutInputData);

	int32 x_lenght = FMath::Abs(InLayoutInputData.Bounds_Max_X - InLayoutInputData.Bounds_Min_X);
	int32 y_lenght = FMath::Abs(InLayoutInputData.Bounds_Max_Y - InLayoutInputData.Bounds_Min_Y);
	int32 lenght = FMath::Max(x_lenght, y_lenght);

	TArray<FIntPoint> GeneratedCells = InLayoutInputData.Cells;
	TArray<FIntPoint> CalculatedCells = TArray<FIntPoint>();
	TArray<FIntPoint> PoolCells = InLayoutInputData.Cells;

	bool bWhileEnable = true;
	while (GeneratedCells.Num() < ToGenerateCellsAmount && bWhileEnable)
	{
		int32 SelectedPoolIndex = Stream.RandRange(0, PoolCells.Num() - 1);
			
		if (!PoolCells.IsValidIndex(SelectedPoolIndex))
		{
			UE_LOG(TerrainGeneratorLog, Error, TEXT("ULayoutObjectRandom::GenerateLayout_Implementation - Invalid index!."));
			bWhileEnable = false;
			break;
		}

		if (CalculatedCells.Contains(PoolCells[SelectedPoolIndex]))
		{
			PoolCells.RemoveAtSwap(SelectedPoolIndex);
			continue;
		}

		TArray<FIntPoint> CurrentGeneratedCells = GenerateCellsAtCellDistance(PoolCells[SelectedPoolIndex], 1, InLayoutInputData);			
		for (const FIntPoint& CurrentGenCell : CurrentGeneratedCells)
		{
			if (!IsCellInBounds(CurrentGenCell, InLayoutInputData))
			{
				continue;
			}

			if (PoolCells.Contains(CurrentGenCell))
			{
				continue;
			}

			PoolCells.AddUnique(CurrentGenCell);
		}

		GeneratedCells.AddUnique(PoolCells[SelectedPoolIndex]);
		CalculatedCells.AddUnique(PoolCells[SelectedPoolIndex]);
		PoolCells.RemoveAtSwap(SelectedPoolIndex);

		if (PoolCells.Num() <= 0)
		{
			UE_LOG(TerrainGeneratorLog, Error, TEXT("ULayoutObjectRandom::GenerateLayout_Implementation - Generation stopped since there is no more room in the quad!."));
			bWhileEnable = false;
			break;
		}
	}
		
	for (const FIntPoint& Cell : GeneratedCells)
	{
		InLayoutInputData.Cells.AddUnique(Cell);
	}
}
