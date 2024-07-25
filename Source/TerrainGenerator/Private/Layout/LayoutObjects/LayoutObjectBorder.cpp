//Copyright 2020 Marchetti Alfredo. All Rights Reserved.
#include "Layout/LayoutObjects/LayoutObjectBorder.h"
#include "Layout/Helpers/TerrainLayoutFunctionLibrary.h"

void ULayoutObjectBorder::GenerateLayout_Implementation(int32 SeedIn, FLayoutInputData& InLayoutInputData)
{
	if (InLayoutInputData.Cells.Num() <= 0)
	{
		return;
	}

	Super::GenerateLayout_Implementation(SeedIn, InLayoutInputData);

	TArray<FIntPoint> InitialCells = InLayoutInputData.Cells;
	for (const FIntPoint& cell : InitialCells)
	{
		TArray<FIntPoint> CurrentGeneratedCells = UTerrainLayoutFunctionLibrary::GetSurroundingCellsOfCell(cell, Range);
		
		for (const FIntPoint& CurrentGenCell : CurrentGeneratedCells)
		{
			if (InLayoutInputData.Cells.Contains(CurrentGenCell))
			{
				continue;
			}

			if (!IsCellInBounds(CurrentGenCell, InLayoutInputData))
			{
				continue;
			}
			
			InLayoutInputData.Cells.AddUnique(CurrentGenCell);
		}
	}
}
