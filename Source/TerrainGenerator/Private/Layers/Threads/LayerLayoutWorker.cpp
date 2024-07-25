//Copyright 2020 Marchetti Alfredo. All Rights Reserved.

#include "Layers/Threads/LayerLayoutWorker.h"
#include "Layers/Subsystems/TerrainLayersSubsystem.h"
#include "Logs/TerrainGeneratorLogs.h"
#include "Layers/Data/TerrainLayerData.h"
#include "Layout/Data/TerrainBaseLayoutData.h"

#pragma region Main Thread Code

FLayerLayoutWorker::FLayerLayoutWorker(const TMap<int32, FTerrainLayerLayout>& LayersLayoutMapIn)
{
	LayersLayoutMap = LayersLayoutMapIn;
}

uint32 FLayerLayoutWorker::Run()
{
	GenerateLayersLayout();

	bIsThreadCompleted = true;
	return 0;
}

void FLayerLayoutWorker::OnThreadEnd()
{
	UTerrainLayersSubsystem* TerrainLayerSubsystem = Cast<UTerrainLayersSubsystem>(ThreadOwner);
	if (!TerrainLayerSubsystem)
	{
		UE_LOG(TerrainGeneratorLog, Error, TEXT("FLayerLayoutWorker::OnThreadEnd - Invalid Layer subsystem."));
		return;
	}
	
	for (const auto& Layout : LayersLayoutMap)
	{
		TerrainLayerSubsystem->LayersLayoutsMap.Add(Layout);
	}
}

void FLayerLayoutWorker::GenerateLayersLayout()
{	
	for (auto& Layout : LayersLayoutMap)
	{
		if(!Layout.Value.LayerData->LayerLayout)
		{
			continue;
		}
		
		// Generate offset centered in the initial cell with the desired quadsize.
		const FIntPoint Offset = FIntPoint(Layout.Value.InitialCell - Layout.Value.LayerData->QuadSize / 2.f);		
		Layout.Value.CellsGenerated = Layout.Value.LayerData->LayerLayout->GenerateLayout(Layout.Value.InitialCell, Stream, Offset);		
	}
}
