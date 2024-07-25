//Copyright 2020 Marchetti Alfredo. All Rights Reserved.
#include "Layout/LayoutThreads/WallLayoutWorker.h"
#include "Layout/Data/TerrainLayoutData.h"
#include "Layout/Helpers/TerrainLayoutFunctionLibrary.h"
#include "Layout/Subsystems/TerrainLayoutSubsystem.h"
#include "Tags/TerrainTags.h"

#pragma region Main Thread Code

FWallLayoutWorker::FWallLayoutWorker(UTerrainLayoutData* TerrainLayoutDataIn, const TMap <FIntPoint, FCellLayout>& CellsLayoutMapIn)
{
	TerrainLayoutData = TerrainLayoutDataIn;
	CellsLayoutMap = CellsLayoutMapIn;
}

uint32 FWallLayoutWorker::Run()
{
	GenerateWallsLayout();

	bIsThreadCompleted = true;
	return 0;
}

void FWallLayoutWorker::OnThreadEnd()
{
	UTerrainLayoutSubsystem* TerrainLayoutSubsystem = Cast<UTerrainLayoutSubsystem>(ThreadOwner);
	if (!TerrainLayoutSubsystem)
	{
		return;
	}

	for (const TPair<FIntPoint, FCellLayout>& pair : GeneratedWallCells)
	{
		if (!TerrainLayoutSubsystem->CellsLayoutMap.Contains(pair.Key))
		{
			TerrainLayoutSubsystem->CellsLayoutMap.Add(pair);
		}
	}
}

void FWallLayoutWorker::GenerateWallsLayout()
{	
	GeneratedWallCells.Empty();

	for (const TPair<FIntPoint, FCellLayout>& pair : CellsLayoutMap)
	{
		TArray<FIntPoint> CurrentGeneratedCells = UTerrainLayoutFunctionLibrary::GetSurroundingCellsOfCell(pair.Key, TerrainLayoutData->WallsRange);

		for (const FIntPoint& CurrentGenCell : CurrentGeneratedCells)
		{
			if (CellsLayoutMap.Contains(CurrentGenCell))
			{
				continue;
			}

			FCellLayout cellLayout = FCellLayout();
			cellLayout.CellID = CurrentGenCell;
			cellLayout.RoomID = pair.Value.RoomID;
			cellLayout.Tags.AddTag(TAG_TERRAIN_CELL_TYPE_WALL);
			GeneratedWallCells.Add(CurrentGenCell, cellLayout);
		}
	}	
}
