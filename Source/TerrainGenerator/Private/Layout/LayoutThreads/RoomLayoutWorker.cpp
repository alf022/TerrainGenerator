//Copyright 2020 Marchetti Alfredo. All Rights Reserved.

#include "Layout/LayoutThreads/RoomLayoutWorker.h"
#include "Layout/Data/TerrainLayoutData.h"
#include "Layout/Data/TerrainLayoutRoomData.h"
#include "Logs/TerrainGeneratorLogs.h"
#include "Layout/Helpers/TerrainLayoutFunctionLibrary.h"
#include "Layout/Subsystems/TerrainLayoutSubsystem.h"

#pragma region Main Thread Code

// This code will be run on the thread that invoked this thread (i.e. game thread)
FRoomLayoutWorker::FRoomLayoutWorker(UTerrainLayoutData* TerrainLayoutDataIn, const TMap<FIntPoint, FRoomLayout>& RoomsLayoutMapIn)
{
	RoomsLayoutMap = RoomsLayoutMapIn;
	TerrainLayoutData = TerrainLayoutDataIn;
}

uint32 FRoomLayoutWorker::Run()
{
	GenerateRoomsLayout();

	UE_LOG(TerrainGeneratorLog, Log, TEXT("FRoomLayoutWorker::GenerateRoomLayoutData - Room layouts generated: %i."), RoomsLayoutMap.Num());
	bIsThreadCompleted = true;//Indicate that the thread data can be collected
	return 0;
}

void FRoomLayoutWorker::OnThreadEnd()
{
	UTerrainLayoutSubsystem* TerrainLayoutSubsystem = Cast<UTerrainLayoutSubsystem>(ThreadOwner);
	if (!TerrainLayoutSubsystem)
	{
		return;
	}

	for (const TPair<FIntPoint, FRoomLayout>& pair : RoomsLayoutMap)
	{
		TerrainLayoutSubsystem->RoomsLayoutMap.Add(pair);
	}
}

void FRoomLayoutWorker::GenerateRoomsLayout()
{	
	for (TPair<FIntPoint, FRoomLayout>& pair : RoomsLayoutMap)
	{			
		int32 RandomRoomIndex = Stream.RandRange(0, TerrainLayoutData->RoomLayouts.Num() - 1);
		if (!TerrainLayoutData->RoomLayouts.IsValidIndex(RandomRoomIndex))
		{
			continue;
		}

		FIntPoint Offset = FIntPoint(
			pair.Key.X * TerrainLayoutData->RoomLayouts[RandomRoomIndex]->RoomLayout->GetQuadSize(),
			pair.Key.Y * TerrainLayoutData->RoomLayouts[RandomRoomIndex]->RoomLayout->GetQuadSize());
		
		pair.Value.Cells = TerrainLayoutData->RoomLayouts[RandomRoomIndex]->RoomLayout->GenerateLayout(pair.Value.InitialCell, Stream, Offset);
		pair.Value.CentralCell = UTerrainLayoutFunctionLibrary::GetIDInAnchor(pair.Value.Cells);
	}
}
