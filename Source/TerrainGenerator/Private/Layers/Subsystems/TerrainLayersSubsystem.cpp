//Copyright 2020 Marchetti Alfredo. All Rights Reserved.

#include "Layers/Subsystems/TerrainLayersSubsystem.h"
#include "Logs/TerrainGeneratorLogs.h"
#include "Layers/Data/TerrainLayerCollectionData.h"
#include "Layers/Data/TerrainLayerData.h"
#include "Layers/Threads/LayerLayoutWorker.h"
#include "Layout/Subsystems/TerrainLayoutSubsystem.h"
#include "MultiThread/TerrainThreadSubsystem.h"
#include "Terrain/Data/TerrainData.h"

void UTerrainLayersSubsystem::GenerateTerrainLayersLayout(UTerrainData* InTerrainData)
{
	if (!InTerrainData)
	{
		UE_LOG(TerrainGeneratorLog, Error, TEXT("UTerrainLayersSubsystem::GenerateTerrainLayerLayout - Invalid Terrain data."));
		return;
	}

	TerrainData = InTerrainData;
	TerrainLayerCollectionData = TerrainData->TerrainLayersCollection;
	
	LayersLayoutsMap.Empty();

	GenerateLayersLayouts();	
	StartLayerLayoutGeneration();
}

void UTerrainLayersSubsystem::GenerateLayersLayouts()
{
	LayersLayoutsMap.Empty();

	for (const auto& LayerData : TerrainLayerCollectionData->TerrainLayers)
	{		
		for (auto ApplicationData : LayerData->InitialCellsApplicationData)
		{
			TArray<FIntPoint> InitialCells = TArray<FIntPoint>();
			InitialCells.Empty();
			GenerateLayoutsInitialCells(ApplicationData, InitialCells);

			for (const auto& InitialCell : InitialCells)
			{
				FTerrainLayerLayout CurrentLayerLayout;
				CurrentLayerLayout.InitialCell = InitialCell;
				CurrentLayerLayout.LayerData = LayerData;

				LayersLayoutsMap.Add(LayersLayoutsMap.Num(), CurrentLayerLayout);			
			}		
		}		
	}
}

void UTerrainLayersSubsystem::GenerateLayoutsInitialCells(const FLayerApplicationData& ApplicationData, TArray<FIntPoint>& CellsOut)
{
	const UTerrainLayoutSubsystem* TerrainLayoutSubsystem = GEngine->GetEngineSubsystem<UTerrainLayoutSubsystem>();
	if (!TerrainLayoutSubsystem)
	{
		UE_LOG(TerrainGeneratorLog, Error, TEXT("UTerrainGeneratorSubsystem::GenerateCellsBiomeLayoutMap - Invalid layout subsystem."));
		return;
	}

	TArray<FIntPoint> CellsGenerated = TArray<FIntPoint>();	
	if(ApplicationData.Mode == ETerrainLayerApplicationMode::Any && ApplicationData.Type == ETerrainLayerApplicationType::Cell)
	{
		TArray<FIntPoint> Cells;
		TerrainLayoutSubsystem->CellsLayoutMap.GetKeys(Cells);
		CellsGenerated = Cells;
		CellsOut = GetCellsByRoll(ApplicationData, CellsGenerated);
	}
	else if(ApplicationData.Mode == ETerrainLayerApplicationMode::Any && ApplicationData.Type == ETerrainLayerApplicationType::InitialRoom)
	{
		CellsGenerated = TerrainLayoutSubsystem->RoomsLayoutMap[TerrainLayoutSubsystem->InitialRoom].Cells;
		CellsOut = GetCellsByRoll(ApplicationData, CellsGenerated);
	}
	else if(ApplicationData.Mode == ETerrainLayerApplicationMode::Any && ApplicationData.Type == ETerrainLayerApplicationType::Room)
	{
		for (const auto& RoomLayout : TerrainLayoutSubsystem->RoomsLayoutMap)
		{
			CellsGenerated.Append(RoomLayout.Value.Cells);
		}
		CellsOut = GetCellsByRoll(ApplicationData, CellsGenerated);
	}
	else if(ApplicationData.Mode == ETerrainLayerApplicationMode::Any && ApplicationData.Type == ETerrainLayerApplicationType::Corridor)
	{
		for (const auto& CorridorLayout : TerrainLayoutSubsystem->CorridorLayouts)
		{
			CellsGenerated.Append(CorridorLayout.Cells);
		}
		CellsOut = GetCellsByRoll(ApplicationData, CellsGenerated);
	}
	if(ApplicationData.Mode == ETerrainLayerApplicationMode::PerEach && ApplicationData.Type == ETerrainLayerApplicationType::Cell)
	{
		// WARNING: Avoid this case if possible		
		TArray<FIntPoint> Cells;
		TerrainLayoutSubsystem->CellsLayoutMap.GetKeys(Cells);
		CellsGenerated = Cells;
		CellsOut = CellsGenerated; // Every cell is selected as a start for a layer
	}
	else if(ApplicationData.Mode == ETerrainLayerApplicationMode::PerEach && ApplicationData.Type == ETerrainLayerApplicationType::InitialRoom)
	{
		const FRoomLayout InitialRoomLayout = TerrainLayoutSubsystem->GetRoomLayout(TerrainLayoutSubsystem->InitialRoom);
		CellsGenerated = InitialRoomLayout.Cells;
		CellsOut.Append(GetCellsByRoll(ApplicationData, CellsGenerated));
	}
	else if(ApplicationData.Mode == ETerrainLayerApplicationMode::PerEach && ApplicationData.Type == ETerrainLayerApplicationType::Room)
	{
		for (const auto& Layout : TerrainLayoutSubsystem->RoomsLayoutMap)
		{
			CellsGenerated = Layout.Value.Cells;
			CellsOut.Append(GetCellsByRoll(ApplicationData, CellsGenerated));
		}
	}
	else if(ApplicationData.Mode == ETerrainLayerApplicationMode::PerEach && ApplicationData.Type == ETerrainLayerApplicationType::Corridor)
	{
		for (const auto& Layout : TerrainLayoutSubsystem->CorridorLayouts)
		{
			CellsGenerated = Layout.Cells;
			CellsOut.Append(GetCellsByRoll(ApplicationData, CellsGenerated));
		}
	}
}

TArray<FIntPoint> UTerrainLayersSubsystem::GetCellsByRoll(const FLayerApplicationData& ApplicationData,
	TArray<FIntPoint>& CellsPool)
{
	//Random suffle
	Algo::Sort(CellsPool, [this] (FIntPoint A, FIntPoint B)
	{
		const float Roll = GetStream().FRandRange(0.f, 1.f);
		return Roll <= .5f;
	});
	
	TArray<FIntPoint> CellsOut;	
	for (int i = 0; i < ApplicationData.MaxLayersAmount; ++i)
	{
		if(!CellsPool.IsValidIndex(CellsOut.Num()))
		{
			return CellsOut;
		}
		
		// No need to roll
		if(i < ApplicationData.MinLayersAmount)
		{
			CellsOut.Add(CellsPool[CellsOut.Num()]);
		}

		// Roll the chance to see if is added.
		const float RollChance = GetStream().FRandRange(0.f, 1.f);
		if(ApplicationData.LayerApplicationChance > RollChance)
		{
			continue;
		}

		CellsOut.Add(CellsPool[CellsOut.Num()]);		
	}

	return CellsOut;
}

void UTerrainLayersSubsystem::StartLayerLayoutGeneration()
{
	if (LayersLayoutsMap.Num() <= 0)
	{
		UE_LOG(TerrainGeneratorLog, Error, TEXT("UTerrainLayersSubsystem::StartLayerLayoutGeneration - No Layer layouts used. Cannot generate."));
		return;
	}
	
	GetTerrainThreadSubsystem()->OnThreadOperationEnd.RemoveAll(this);
	GetTerrainThreadSubsystem()->OnThreadOperationEnd.AddDynamic(this, &UTerrainLayersSubsystem::OnLayerLayoutEnd);
	
	const int32 Threads = FMath::Clamp(TerrainData->MaxThreads, 1, TerrainData->MaxThreads);
	int32 TotalPerThread = LayersLayoutsMap.Num();
	if(Threads > 1)
	{
		TotalPerThread = LayersLayoutsMap.Num() / (Threads - 1);
	}	
	
	TArray <FBaseTerrainWorker*> CurrentActiveThreads;
	TMap<int32, FTerrainLayerLayout> CurrentLayouts;
		
	int32 i = 0;
	for (const auto& pair : LayersLayoutsMap)
	{
		CurrentLayouts.Add(pair);
	
		if (CurrentLayouts.Num() >= TotalPerThread || i == LayersLayoutsMap.Num() - 1)
		{
			FLayerLayoutWorker* Worker = new FLayerLayoutWorker(		
				CurrentLayouts
			);
	
			CurrentActiveThreads.Add(Worker);
			CurrentLayouts.Empty();
		}
	
		i++;
	}
	
	LayersLayoutsMap.Empty(); // We dont need this data anymore, after this we use the generatedlayouts
	GetTerrainThreadSubsystem()->StartThreads(this, GetStream(), CurrentActiveThreads);
}

void UTerrainLayersSubsystem::OnLayerLayoutEnd()
{
	GetTerrainThreadSubsystem()->OnThreadOperationEnd.RemoveAll(this);

	ApplyLayers();
	
	OnLayersLayoutGeneratedED.Broadcast();
	OnLayersLayoutGenerated.Broadcast();

#if WITH_EDITOR	
		
	if (IsDebug())
	{
		if (GetState() == ETerrainGen_State::LayersLayout)
		{			
			return;
		}
	}

#endif
}

void UTerrainLayersSubsystem::ApplyLayers()
{
	UTerrainLayoutSubsystem* TerrainLayoutSubsystem = GEngine->GetEngineSubsystem<UTerrainLayoutSubsystem>();
	if (!TerrainLayoutSubsystem)
	{
		UE_LOG(TerrainGeneratorLog, Error, TEXT("UTerrainGeneratorSubsystem::ApplyLayers - Invalid layout subsystem."));
		return;
	}
	
	const int32 MaxOrder = GetMaxLayerApplicationOrder();
	for (int i = 0; i <= MaxOrder; ++i)
	{
		for (const auto& Layout : LayersLayoutsMap)
		{
			for (const auto& CellID : Layout.Value.CellsGenerated)
			{
				if(!TerrainLayoutSubsystem->IsCellIDValid(CellID))
				{
					continue;
				}
		
				FCellLayout CellLayout = TerrainLayoutSubsystem->GetCellLayout(CellID);

				// Check tags in cell
				if(!CellLayout.Tags.HasAll(Layout.Value.LayerData->ApplicationRequiredTags))
				{
					continue;
				}				
				if(CellLayout.Tags.HasAny(Layout.Value.LayerData->ApplicationIgnoredTags))
				{
					continue;
				}

				if(!Layout.Value.LayerData->LayerTag.IsValid() || Layout.Value.LayerData->LayerTag == FGameplayTag::EmptyTag)
				{
					UE_LOG(TerrainGeneratorLog, Error, TEXT("UTerrainGeneratorSubsystem::ApplyLayers - Invalid or empty layer tag!."));
				}
				
				FGameplayTagContainer Container = Layout.Value.LayerData->LayerTag.GetSingleTagContainer();
				Container.AppendTags(Layout.Value.LayerData->LayerAdditionalTags);
				TerrainLayoutSubsystem->AppendTagsToCell(CellID, Container);
			}		
		}		
	}	
}

int32 UTerrainLayersSubsystem::GetMaxLayerApplicationOrder() const
{
	int32 MaxOrder = 0;
	for (const auto& LayerData : TerrainLayerCollectionData->TerrainLayers)
	{
		if (LayerData->ApplicationOrder > MaxOrder)
		{
			MaxOrder = LayerData->ApplicationOrder;
		}
	}

	return MaxOrder;
}
