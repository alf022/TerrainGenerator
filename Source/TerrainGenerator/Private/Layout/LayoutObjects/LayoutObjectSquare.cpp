//Copyright 2020 Marchetti Alfredo. All Rights Reserved.
#include "Layout/LayoutObjects/LayoutObjectSquare.h"

void ULayoutObjectSquare::GenerateLayout_Implementation(int32 SeedIn, FLayoutInputData& InLayoutInputData)
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

	int32 CurrentDistanceToCenter = 0;

	for (int32 i = 0; i < lenght; i++)
	{
		CurrentDistanceToCenter++;

		//Non corners first
		TArray<FIntPoint> cells = GenerateCellsAtCellDistance(InLayoutInputData.Cells[0], CurrentDistanceToCenter, InLayoutInputData);//TODO: ASsumes that cells has only 1, this needs to be changed
		for (const FIntPoint& cell : cells)
		{
			if (!IsCellInBounds(cell, InLayoutInputData))
			{
				continue;
			}

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

		//Corners only
		cells = GenerateCellsAtCellDistance(InLayoutInputData.Cells[0], CurrentDistanceToCenter, InLayoutInputData, true);//TODO: ASsumes that cells has only 1, this needs to be changed
		for (const FIntPoint& cell : cells)
		{
			if (!IsCellInBounds(cell, InLayoutInputData))
			{
				continue;
			}

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
