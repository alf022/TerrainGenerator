//Copyright 2020 Marchetti Alfredo. All Rights Reserved.
#include "Layout/Subsystems/TerrainLayoutSubsystem.h"
#include "Logs/TerrainGeneratorLogs.h"
#include "Terrain/Data/TerrainData.h"
#include "Layout/Data/TerrainLayoutData.h"
#include "Layout/Data/TerrainLayoutRoomData.h"
#include "Tags/TerrainTags.h"
#include "Kismet/KismetMathLibrary.h"
#include "MultiThread/TerrainThreadSubsystem.h"
#include "Layout/LayoutThreads/RoomLayoutWorker.h"
#include "Layout/LayoutThreads/RoomMovementWorker.h"
#include "Layout/LayoutThreads/CorridorLayoutWorker.h"
#include "Layout/LayoutThreads/WallLayoutWorker.h"
#include "Layout/Helpers/TerrainLayoutFunctionLibrary.h"
#include "Terrain/Types/TerrainGeneratorTypes.h"

void UTerrainLayoutSubsystem::Deinitialize()
{
	ClearData();	
	Super::Deinitialize();
}

void UTerrainLayoutSubsystem::ClearData()
{
	TerrainLayoutData = nullptr;
	RoomsLayoutMap.Empty();
	CorridorLayouts.Empty();
	CellsLayoutMap.Empty();
}

FIntPoint UTerrainLayoutSubsystem::GetInitialRoom() const
{
	return InitialRoom;
}

TMap<FIntPoint, FRoomLayout> UTerrainLayoutSubsystem::GetRoomsLayoutMap() const
{
	return RoomsLayoutMap;
}

TMap<FIntPoint, FCellLayout> UTerrainLayoutSubsystem::GetCellsLayoutMap() const
{
	return CellsLayoutMap;
}

bool UTerrainLayoutSubsystem::IsCellIDValid(const FIntPoint& InCellID) const
{
	return CellsLayoutMap.Contains(InCellID);
}

FCellLayout UTerrainLayoutSubsystem::GetCellLayout(const FIntPoint& InCellsID) const
{
	if(!CellsLayoutMap.Contains(InCellsID))
	{
		return FCellLayout();
	}
	
	return CellsLayoutMap[InCellsID];
}

FRoomLayout UTerrainLayoutSubsystem::GetRoomLayout(const FIntPoint& InRoomID) const
{
	if(!RoomsLayoutMap.Contains(InRoomID))
	{
		return FRoomLayout();
	}
	
	return RoomsLayoutMap[InRoomID];
}

void UTerrainLayoutSubsystem::AppendTagsToCell(const FIntPoint& InCellID, FGameplayTagContainer Tags)
{
	if(!IsCellIDValid(InCellID))
	{
		return;
	}

	CellsLayoutMap[InCellID].Tags.AppendTags(Tags);
}

FVector UTerrainLayoutSubsystem::GetCellWorldPosition(const FIntPoint& InCellsID, const FVector2D InAnchor) const
{
	if (!TerrainLayoutData)
	{
		return FVector::ZeroVector;
	}

	FVector Position;

	//Axis must be reversed for this to work(See landscape placement logic)
	Position.X = InCellsID.Y * TerrainData->CellSize + TerrainData->CellSize * InAnchor.Y; //Show the location based on anchor
	Position.Y = InCellsID.X * TerrainData->CellSize + TerrainData->CellSize * InAnchor.X;

	Position.Z = 0;

	return Position;
}

float UTerrainLayoutSubsystem::GetWorldDistanceBetweenRooms(const FIntPoint& RoomA, const FIntPoint& RoomB) const
{	
	if(!RoomsLayoutMap.Contains(RoomA) || !RoomsLayoutMap.Contains(RoomB))
	{
		return 0;
	}

	const FVector RoomACenter = GetCellWorldPosition(RoomsLayoutMap[RoomA].CentralCell,{ .5,.5 });
	const FVector RoomBCenter = GetCellWorldPosition(RoomsLayoutMap[RoomB].CentralCell, { .5,.5 });
	return FVector::Dist2D(RoomACenter, RoomBCenter);
}

void UTerrainLayoutSubsystem::GenerateTerrainLayout(UTerrainData* InTerrainData)
{
	if (!InTerrainData)
	{
		UE_LOG(TerrainGeneratorLog, Error, TEXT("UTerrainLayoutSubsystem::GenerateTerrainLayout - Invalid terrain data."));
		return;
	}

	if (!InTerrainData->TerrainLayoutData)
	{
		UE_LOG(TerrainGeneratorLog, Error, TEXT("UTerrainLayoutSubsystem::GenerateTerrainLayout - Invalid layout data."));
		return;
	}

	TerrainData = InTerrainData;
	TerrainLayoutData = InTerrainData->TerrainLayoutData;

	CellsLayoutMap.Empty();
	RoomsLayoutMap.Empty();
	CorridorLayouts.Empty();
	
	GenerateInitialRoomsLayout();
	StartRoomLayoutGeneration();
}

void UTerrainLayoutSubsystem::GenerateInitialRoomsLayout()
{	
	FIntPoint InitialCell = FIntPoint();
	TArray <FIntPoint> InitialLayout = TerrainLayoutData->InitialRoomsLayout->GenerateLayout(InitialCell, GetStream());

	for (const FIntPoint& RoomsInitialLayout : InitialLayout)
	{
		FRoomLayout Layout = FRoomLayout();	
		RoomsLayoutMap.Add(RoomsInitialLayout, Layout);
	}	
}

void UTerrainLayoutSubsystem::StartRoomLayoutGeneration()
{
	if (TerrainLayoutData->RoomLayouts.Num() <= 0)
	{
		UE_LOG(TerrainGeneratorLog, Error, TEXT("UTerrainLayoutSubsystem::StartRoomLayoutGeneration - No room layouts used. Cannot generate rooms."));
		return;
	}
	
	GetTerrainThreadSubsystem()->OnThreadOperationEnd.RemoveAll(this);
	GetTerrainThreadSubsystem()->OnThreadOperationEnd.AddDynamic(this, &UTerrainLayoutSubsystem::OnRoomLayoutEnd);

	//This is the amount of rooms per thread. The Idea is to not mass too many threads. The threads handle a few ideally
	const int32 Threads = FMath::Clamp(TerrainData->MaxThreads, 1, TerrainData->MaxThreads);
	const int32 TotalPerThread = RoomsLayoutMap.Num() / (Threads - 1);

	TArray <FBaseTerrainWorker*> CurrentActiveThreads;
	TMap<FIntPoint, FRoomLayout> CurrentLayouts;

	int32 i = 0;
	for (const TPair<FIntPoint, FRoomLayout>& pair : RoomsLayoutMap)
	{		
		CurrentLayouts.Add(pair);

		if (CurrentLayouts.Num() >= TotalPerThread || i == RoomsLayoutMap.Num() - 1)
		{
			FRoomLayoutWorker* Worker = new FRoomLayoutWorker(
				TerrainLayoutData,
				CurrentLayouts
			);

			CurrentActiveThreads.Add(Worker);
			CurrentLayouts.Empty();
		}

		i++;
	}

	GetTerrainThreadSubsystem()->StartThreads(this, GetStream(), CurrentActiveThreads);
}

void UTerrainLayoutSubsystem::OnRoomLayoutEnd()
{
	GetTerrainThreadSubsystem()->OnThreadOperationEnd.RemoveAll(this);
	
#if WITH_EDITOR	
	if (IsDebug())
	{	
		if (GetState() == ETerrainGen_State::InitialLayout)
		{		
			for (const TPair<FIntPoint, FRoomLayout>& pair : RoomsLayoutMap)
			{				
				for (const FIntPoint& cellID : pair.Value.Cells)
				{
					FCellLayout CellLayout = FCellLayout();
					CellLayout.RoomID = pair.Key;
					CellLayout.CellID = cellID;

					CellLayout.Tags.AddTag(TAG_TERRAIN_CELL_TYPE_ROOM);

					if (RoomsLayoutMap[pair.Key].InitialCell == cellID)
					{
						CellLayout.Tags.AddTag(TAG_TERRAIN_CELL_LAYOUT_INITIAL);
					}

					if (RoomsLayoutMap[pair.Key].CentralCell == cellID)
					{
						CellLayout.Tags.AddTag(TAG_TERRAIN_CELL_LAYOUT_CENTRAL);
					}					

					CellsLayoutMap.Add(cellID, CellLayout);
				}
			}
#if WITH_EDITOR
			OnInitialLayoutGeneratedED.Broadcast();
#endif
			OnInitialLayoutGenerated.Broadcast();
			return;
		}	
	}
#endif

	StartRoomMovement();		
}

void UTerrainLayoutSubsystem::StartRoomMovement()
{
	GetTerrainThreadSubsystem()->OnThreadOperationEnd.RemoveAll(this);
	GetTerrainThreadSubsystem()->OnThreadOperationEnd.AddDynamic(this, &UTerrainLayoutSubsystem::OnRoomMovementEnd);

	TArray<FIntPoint> CentralCells = TArray<FIntPoint>();
	TArray<FIntPointPair> RoomCentralCellsPairs = GetRoomsCentralCells(CentralCells);

	FIntPoint InitialRoomCell = UTerrainLayoutFunctionLibrary::GetIDInAnchor(CentralCells, TerrainLayoutData->InitialRoomsLayout->InitialCellAnchor);
	
	//Sort the rooms + intial cell ID, based on Initial cell distance to the initial cell room.
	Algo::Sort(RoomCentralCellsPairs, [this, &InitialRoomCell] (FIntPointPair A, FIntPointPair B)
	{
		const int32 ADistance = UTerrainLayoutFunctionLibrary::GetCellDistanceBetweenCells(InitialRoomCell, A.PointB);
		const int32 BDistance = UTerrainLayoutFunctionLibrary::GetCellDistanceBetweenCells(InitialRoomCell, B.PointB);
		return ADistance <= BDistance;
	});
		
	InitialRoom = RoomCentralCellsPairs[0].PointA;
	const FIntPointPair InitialRoomPair = RoomCentralCellsPairs[0];
	RoomCentralCellsPairs.RemoveAt(0);

	const int32 Threads = FMath::Clamp(TerrainData->MaxThreads, 1, TerrainData->MaxThreads);
	const float ThreadAngle = 360 / Threads;
	TArray <FBaseTerrainWorker*> ActiveThreads;
	TArray <FIntPointPair> CurrentLayouts;	
	TArray <FIntPoint> UsedLayouts;

	TArray<FIntPoint> BorderCells = GetRoomInitialCollisionCells(InitialRoomPair.PointA);

	for (int32 i = 0; i < Threads + 1; i++)
	{
		CurrentLayouts.Empty();
		CurrentLayouts.Add(InitialRoomPair); //All layouts calculate border cells with this?

		for (const FIntPointPair& pointPair : RoomCentralCellsPairs)
		{
			if (UsedLayouts.Contains(pointPair.PointA))
			{
				continue;
			}

			const float Angle = UTerrainLayoutFunctionLibrary::GetRoomVectorAngle(pointPair.PointB, InitialRoomCell);
			
			if (Angle >= i * ThreadAngle && Angle < (i + 1) * ThreadAngle )
			{
				CurrentLayouts.Add(pointPair);
				UsedLayouts.Add(pointPair.PointA);
			}			
		}

		//Last thread will also pass initial room, so it can handle all the tagging. Other threads just use the border cells of the initial room
		if (i == Threads)
		{
			BorderCells.Empty(); //Because i pass the room, i cannot pass the border cells.
			CurrentLayouts.Insert(InitialRoomPair, 0); 
		}
		
		FRoomMovementWorker* Worker = new FRoomMovementWorker(
			InitialRoom,
			BorderCells,
			CurrentLayouts,
			TerrainLayoutData);

		ActiveThreads.Add(Worker);	
	}
	
	GetTerrainThreadSubsystem()->StartThreads(this, GetStream(), ActiveThreads);
}

TArray<FIntPointPair> UTerrainLayoutSubsystem::GetRoomsCentralCells(TArray<FIntPoint>& CentralCellsOut) const
{
	TArray<FIntPointPair> RoomCentralCells;
	RoomCentralCells.Empty();
	for (const TPair<FIntPoint, FRoomLayout>& pair : RoomsLayoutMap)
	{
		FIntPointPair Pair = FIntPointPair();
		Pair.PointA = pair.Key;
		Pair.PointB = pair.Value.CentralCell;
		RoomCentralCells.Add(Pair);
		CentralCellsOut.Add(pair.Value.CentralCell);
	}

	return RoomCentralCells;
}

TArray<FIntPoint> UTerrainLayoutSubsystem::GetRoomInitialCollisionCells(FIntPoint RoomID) const
{
	TArray <FIntPoint> BorderCells = TArray <FIntPoint>();

	if (!RoomsLayoutMap.Contains(RoomID))
	{
		return BorderCells;
	}
		
	for (const FIntPoint& cellID : RoomsLayoutMap[RoomID].Cells)
	{
		if ((cellID.X % TerrainLayoutData->RoomSeparationPatternIndex && !(cellID.Y % TerrainLayoutData->RoomSeparationPatternIndex)) ||
			(!(cellID.X % TerrainLayoutData->RoomSeparationPatternIndex) && (cellID.Y % TerrainLayoutData->RoomSeparationPatternIndex)))
		{
			BorderCells.Add(cellID);
		}
	}

	return BorderCells;

}

void UTerrainLayoutSubsystem::OnRoomMovementEnd()
{
	GetTerrainThreadSubsystem()->OnThreadOperationEnd.RemoveAll(this);
	
#if WITH_EDITOR
	OnInitialLayoutMovementCompletedED.Broadcast();
#endif	
	OnInitialLayoutMovementCompleted.Broadcast();

#if WITH_EDITOR		
	if (IsDebug())
	{
		if (GetState() == ETerrainGen_State::InitialLayoutMovement)
		{
			return;
		}
	}
#endif

	StartCorridorsLayoutGeneration();
}

void UTerrainLayoutSubsystem::StartCorridorsLayoutGeneration()
{
	GetTerrainThreadSubsystem()->OnThreadOperationEnd.RemoveAll(this);
	GetTerrainThreadSubsystem()->OnThreadOperationEnd.AddDynamic(this, &UTerrainLayoutSubsystem::OnCorridorLayoutEnd);

	TArray<FTerrain_RoomDistance> InitialCorridorsLayoutData = GenerateInitialCorridorsLayoutData();

	//This is the amount of corridors per thread. The Idea is to not mass too many threads. The threads handle a few corridors ideally
	const int32 Threads = FMath::Clamp(TerrainData->MaxThreads, 1, TerrainData->MaxThreads);
	const int32 TotalPerThread = InitialCorridorsLayoutData.Num() / (Threads - 1);

	TArray <FBaseTerrainWorker*> CorridorLayoutActiveThreads;
	TArray<FTerrain_RoomDistance> CurrentLayouts = TArray<FTerrain_RoomDistance>();

	for (int32 i = 0; i < InitialCorridorsLayoutData.Num(); i++)
	{
		CurrentLayouts.Add(InitialCorridorsLayoutData[i]);

		if (CurrentLayouts.Num() >= TotalPerThread || i == InitialCorridorsLayoutData.Num() - 1)
		{
			FCorridorLayoutWorker* Worker = new FCorridorLayoutWorker(
				TerrainData->CellSize,
				CurrentLayouts,
				TerrainLayoutData, 			
				RoomsLayoutMap,
				CellsLayoutMap);

			CorridorLayoutActiveThreads.Add(Worker);	
			CurrentLayouts.Empty();
		}
	}	

	GetTerrainThreadSubsystem()->StartThreads(this, GetStream(), CorridorLayoutActiveThreads);
}

TArray<FTerrain_RoomDistance> UTerrainLayoutSubsystem::GenerateInitialCorridorsLayoutData()
{
	TArray<FTerrain_RoomDistance> ThreeCorridorsLayout = TArray<FTerrain_RoomDistance>();
	TArray<FIntPoint> ConnectedRooms = TArray<FIntPoint>();
	ConnectedRooms.Add(InitialRoom);

	int32 WhileCounter = 0;
	TArray<FTerrain_RoomDistance> AllDistances = GetAllRoomDistanceData();	
	while (ConnectedRooms.Num() < RoomsLayoutMap.Num())
	{
		WhileCounter++;
		for (const FTerrain_RoomDistance& distance : AllDistances)
		{
			bool bUsedDistance = false;
			if (!ConnectedRooms.Contains(distance.RoomA) && ConnectedRooms.Contains(distance.RoomB))
			{
				ConnectedRooms.Add(distance.RoomA);

				if (distance.Distance > 0)  //Avoid creating a corridor if the rooms are next to each other, they are already connected
				{
					ThreeCorridorsLayout.Add(distance);
				}

				bUsedDistance = true;
			}
			else if (!ConnectedRooms.Contains(distance.RoomB) && ConnectedRooms.Contains(distance.RoomA))
			{
				ConnectedRooms.Add(distance.RoomB);
				
				if (distance.Distance > 0)  //Avoid creating a corridor if the rooms are next to each other, they are already connected
				{
					ThreeCorridorsLayout.Add(distance);
				}
			
				bUsedDistance = true;
			}

			if (bUsedDistance)
			{			
				break;
			}
		}

		if (WhileCounter > RoomsLayoutMap.Num() * 2)
		{
			UE_LOG(TerrainGeneratorLog, Error, TEXT("UTerrainLayoutSubsystem::GenerateInitialCorridorsLayoutData - Corridors Minimun span three took too long togenerate. Operation canceled. Increase CorridorDetectionMultiplier value."));
			break;
		}
	}
		
	if (TerrainLayoutData->CircularCorridorsAmountPercent > 0)
	{
		AddUnusedCorridors(AllDistances, ThreeCorridorsLayout); //The circular corridors can repeat rooms, i could filter somehow, but is not really worth it
	}

	return ThreeCorridorsLayout;
}

TArray<FTerrain_RoomDistance> UTerrainLayoutSubsystem::GetAllRoomDistanceData() const
{	
	TArray<FTerrain_RoomDistance> AllDistances = TArray < FTerrain_RoomDistance>();
	for (const TPair<FIntPoint, FRoomLayout>& pair : RoomsLayoutMap)
	{
		AllDistances.Append(GetRoomDistancesInNearArea(pair.Key));
	}

	Algo::Sort(AllDistances, [](const FTerrain_RoomDistance& A, const FTerrain_RoomDistance& B)
		{
			return A.Distance <= B.Distance;
		});
	
	return AllDistances;
}

TArray<FTerrain_RoomDistance> UTerrainLayoutSubsystem::GetRoomDistancesInNearArea(const FIntPoint& StartRoom) const
{
	if(!RoomsLayoutMap.Contains(StartRoom))
	{
		return TArray<FTerrain_RoomDistance>();
	}

	int32 AverageRoomCells = 0;
	for (const UTerrainLayoutRoomData* RoomLayout : TerrainLayoutData->RoomLayouts)
	{
		AverageRoomCells += RoomLayout->RoomLayout->GetMaxCells();
	}
	AverageRoomCells /= TerrainLayoutData->RoomLayouts.Num();
	AverageRoomCells = UKismetMathLibrary::Sqrt(AverageRoomCells);

	const int32 QuadSize = (TerrainLayoutData->MaxRoomDistance + AverageRoomCells) * TerrainLayoutData->CorridorDetectionMultiplier;
	const FIntPoint StartCell = RoomsLayoutMap[StartRoom].CentralCell;

	const float MinX = StartCell.X - QuadSize;
	const float MaxX = StartCell.X + QuadSize - 1;
	const float MinY = StartCell.Y - QuadSize;
	const float MaxY = StartCell.Y + QuadSize - 1;
		
	TArray<FTerrain_RoomDistance> DistancesOut = TArray<FTerrain_RoomDistance>();
	for (int32 i = MinX; i < MaxX; i++)
	{
		for (int32 j = MinY; j < MaxY; j++)
		{		
			if (!CellsLayoutMap.Contains(FIntPoint(i, j)))
			{
				continue;
			}

			if (StartRoom == CellsLayoutMap[FIntPoint(i, j)].RoomID)
			{
				continue;
			}	

			if(!RoomsLayoutMap.Contains(CellsLayoutMap[FIntPoint(i, j)].RoomID))
			{
				continue;
			}

			FTerrain_RoomDistance distance = FTerrain_RoomDistance();
			distance.RoomA = StartRoom;
			distance.RoomB = CellsLayoutMap[FIntPoint(i, j)].RoomID;

			if (!DistancesOut.Contains(distance))
			{
				distance.Distance = GetWorldDistanceBetweenRooms(distance.RoomA, distance.RoomB);
				DistancesOut.Add(distance);
			}		
		}
	}
	
	return DistancesOut;
}

void UTerrainLayoutSubsystem::AddUnusedCorridors(TArray<FTerrain_RoomDistance>& UnusedCorridorsIn, TArray<FTerrain_RoomDistance>& ThreeCorridorsOut)
{
	if (TerrainLayoutData->CircularCorridorsAmountPercent <= 0)
	{
		return;
	}

	TArray<FTerrain_RoomDistance> FilteredCorridors = TArray<FTerrain_RoomDistance>();

	if (TerrainLayoutData->MaxCircularCorridorsCellDistance > 0)
	{
		for (const FTerrain_RoomDistance& RoomDistance : UnusedCorridorsIn)
		{
			if ((RoomDistance.Distance / TerrainData->CellSize) < TerrainLayoutData->MaxCircularCorridorsCellDistance)
			{
				FilteredCorridors.Add(RoomDistance);
			}
		}
	}
	else 
	{
		FilteredCorridors = UnusedCorridorsIn;
	}

	Algo::Sort(FilteredCorridors, [this] (FTerrain_RoomDistance A, FTerrain_RoomDistance B)
	{
		return GetStream().FRand() < .5f;
	});

	const int32 CorridorsAmount = TerrainLayoutData->CircularCorridorsAmountPercent * RoomsLayoutMap.Num();
	for (int i = 0; i < CorridorsAmount; i++)
	{
		const int32 RandomRoll = GetStream().RandRange(0, FilteredCorridors.Num());
		if (FilteredCorridors.IsValidIndex(RandomRoll))
		{
			ThreeCorridorsOut.Add(FilteredCorridors[RandomRoll]);
		}
	}
}

void UTerrainLayoutSubsystem::OnCorridorLayoutEnd()
{
	GetTerrainThreadSubsystem()->OnThreadOperationEnd.RemoveAll(this);

#if WITH_EDITOR
	OnCorridorLayoutGeneratedED.Broadcast();
#endif
	OnCorridorLayoutGenerated.Broadcast();
	
#if WITH_EDITOR		
	if (IsDebug())
	{
		if (GetState() == ETerrainGen_State::CorridorsLayout)
		{
			return;
		}
	}
#endif

	StartWallsLayoutGeneration();	
}

void UTerrainLayoutSubsystem::StartWallsLayoutGeneration()
{
	GetTerrainThreadSubsystem()->OnThreadOperationEnd.RemoveAll(this);
	GetTerrainThreadSubsystem()->OnThreadOperationEnd.AddDynamic(this, &UTerrainLayoutSubsystem::OnWallsLayoutEnd);

	const int32 Threads = FMath::Clamp(TerrainData->MaxThreads, 1, TerrainData->MaxThreads);
	const int32 TotalPerThread = CellsLayoutMap.Num() / (Threads - 1);

	TArray <FBaseTerrainWorker*> ActiveThreads;
	TMap<FIntPoint, FCellLayout> CurrentLayouts;

	int32 Index = -1;
	for (const TPair<FIntPoint, FCellLayout>& pair : CellsLayoutMap)
	{
		Index++;

		CurrentLayouts.Add(pair);

		if (CurrentLayouts.Num() >= TotalPerThread || Index == CellsLayoutMap.Num() - 1)
		{
			FWallLayoutWorker* Worker = new FWallLayoutWorker(	
				TerrainLayoutData,
				CurrentLayouts);

			ActiveThreads.Add(Worker);
			CurrentLayouts.Empty();
		}
	}

	GetTerrainThreadSubsystem()->StartThreads(this, GetStream(), ActiveThreads);
}

void UTerrainLayoutSubsystem::OnWallsLayoutEnd()
{
	GetTerrainThreadSubsystem()->OnThreadOperationEnd.RemoveAll(this);

#if WITH_EDITOR
	OnWallsLayoutGeneratedED.Broadcast();
#endif
	OnWallsLayoutGenerated.Broadcast();

#if WITH_EDITOR		
	if (IsDebug())
	{
		if (GetState() == ETerrainGen_State::WallsLayout)
		{
			return;
		}
	}
#endif

	OnLayoutGenerationEnd();
}

void UTerrainLayoutSubsystem::OnLayoutGenerationEnd()
{
	CalculateRoomsDungeonDepth();
	CalculateCellsLayoutDepth();

#if WITH_EDITOR
	OnLayoutGeneratedED.Broadcast();
#endif
	OnLayoutGenerated.Broadcast();
}

void UTerrainLayoutSubsystem::CalculateRoomsDungeonDepth()
{
	//@TODO: Calculate Distance to the start cell in "Rooms" for all rooms. (Min rooms the player has to pass to find the room)
	// Might be useful data for users
}

void UTerrainLayoutSubsystem::CalculateCellsLayoutDepth()
{
	//@TODO: Calculate Min Distance from the cell to a wall
	// Might be useful data for users
}
