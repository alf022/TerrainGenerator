//Copyright 2020 Marchetti Alfredo. All Rights Reserved.
#include "Layout/LayoutObjects/LayoutObjectDiamond.h"

void ULayoutObjectDiamond::GenerateLayout_Implementation(int32 SeedIn, FLayoutInputData& InLayoutInputData)
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
	TArray<FIntPoint> LoopGeneratedCells = TArray<FIntPoint>();
	
	for (int32 i = 0; i < lenght; i++)
	{
		LoopGeneratedCells.Empty();

		for (const FIntPoint& cell : GeneratedCells)
		{
			//Get Non corners adyacents of all current cells
			TArray<FIntPoint> CurrentGeneratedCells = GenerateCellsAtCellDistance(cell, 1, InLayoutInputData);
			for (const FIntPoint& CurrentGenCell : CurrentGeneratedCells)
			{
				if (GeneratedCells.Contains(CurrentGenCell))
				{
					continue;
				}

				if (!IsCellInBounds(CurrentGenCell, InLayoutInputData))
				{
					continue;
				}

				LoopGeneratedCells.AddUnique(CurrentGenCell);				
			}			
		}

		Algo::Sort(LoopGeneratedCells, [this](FIntPoint A, FIntPoint B)
			{
				return Stream.FRand() < .5f;
			});
		

		for (const FIntPoint& cell : LoopGeneratedCells)
		{
			GeneratedCells.AddUnique(cell);

			if (GeneratedCells.Num() >= ToGenerateCellsAmount)
			{
				break;
			}
		}
		
		if (GeneratedCells.Num() >= ToGenerateCellsAmount)
		{
			break;
		}
	}	
	
	for (const FIntPoint& Cell : GeneratedCells)
	{
		InLayoutInputData.Cells.AddUnique(Cell);
	}
}
