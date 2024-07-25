//Copyright 2020 Marchetti Alfredo. All Rights Reserved.

#include "Layout/LayoutThreads/CorridorLayoutWorker.h"
#include "Layout/Helpers/TerrainLayoutFunctionLibrary.h"
#include "Logs/TerrainGeneratorLogs.h"
#include "Tags/TerrainTags.h"
#include "Layout/Data/TerrainLayoutData.h"
#include "Layout/Subsystems/TerrainLayoutSubsystem.h"

#pragma region Main Thread Code

FCorridorLayoutWorker::FCorridorLayoutWorker(float CellSizeIn, TArray<FTerrain_RoomDistance> StartEndRoomIn, UTerrainLayoutData* TerrainLayoutDataIn, const TMap<FIntPoint, FRoomLayout>& RoomsLayoutMapIn, const TMap<FIntPoint, FCellLayout>& CellsLayoutMapIn)
{
	CellSize = CellSizeIn;
	StartEndRoom = StartEndRoomIn;
	TerrainLayoutData = TerrainLayoutDataIn;
	RoomsLayoutMap = RoomsLayoutMapIn;
	CellsLayoutMap = CellsLayoutMapIn;
}

uint32 FCorridorLayoutWorker::Run()
{
	GeneratedCorridorLayout.Empty();

	FCorridorLayout Layout = FCorridorLayout();
	bool bLayoutGenerated = false;
	for (const FTerrain_RoomDistance StartEnd : StartEndRoom)
	{
		Layout = GenerateCorridorLayout(StartEnd, bLayoutGenerated);
	
		if (bLayoutGenerated)
		{
			GeneratedCorridorLayout.Add(Layout);
		}		
	}
	
	UE_LOG(TerrainGeneratorLog, Log, TEXT("FCorridorLayoutWorker::GenerateCorridorLayoutData - Corridor layouts generated: %i."), GeneratedCorridorLayout.Num());
	bIsThreadCompleted = true;
	return 0;
}

void FCorridorLayoutWorker::OnThreadEnd() 
{
	UTerrainLayoutSubsystem* TerrainLayoutSubsystem = Cast<UTerrainLayoutSubsystem>(ThreadOwner);
	if (!TerrainLayoutSubsystem)
	{
		return;
	}

	TerrainLayoutSubsystem->CorridorLayouts.Append(GeneratedCorridorLayout); // Keep all corridor generated layouts just for information sake.
	
	for (const FCorridorLayout& layout : GeneratedCorridorLayout)
	{
		for (const FIntPoint& cell : layout.Cells)
		{				
			FCellLayout CellLayout = FCellLayout();
			if (TerrainLayoutSubsystem->CellsLayoutMap.Contains(cell))
			{
				CellLayout = TerrainLayoutSubsystem->CellsLayoutMap[cell];
			}

			CellLayout.CellID = cell;
			CellLayout.Tags.AddTag(TAG_TERRAIN_CELL_TYPE_CORRIDOR);

			if (cell == layout.EndCellId)
			{
				CellLayout.Tags.AddTag(TAG_TERRAIN_CELL_LAYOUT_DOOR_ENDCORRIDOR);
			}
			else if (cell == layout.StartCellId)
			{
				CellLayout.Tags.AddTag(TAG_TERRAIN_CELL_LAYOUT_DOOR_STARTCORRIDOR);
			}

			TerrainLayoutSubsystem->CellsLayoutMap.Add(cell, CellLayout);
		}
	}
}

FCorridorLayout FCorridorLayoutWorker::GenerateCorridorLayout(const FTerrain_RoomDistance& StartEndRoomIn, bool& SuccesOut) const
{
	if (!RoomsLayoutMap.Contains(StartEndRoomIn.RoomA))
	{
		UE_LOG(TerrainGeneratorLog, Error, TEXT("FCorridorLayoutWorker::GenerateCorridorLayoutData - Could not find layout data for start room."));
		SuccesOut = false;
		return FCorridorLayout();
	}

	if (!RoomsLayoutMap.Contains(StartEndRoomIn.RoomB))
	{
		UE_LOG(TerrainGeneratorLog, Error, TEXT("FCorridorLayoutWorker::GenerateCorridorLayoutData - Could not find layout data for end room."));
		SuccesOut = false;
		return FCorridorLayout();
	}

	FCorridorLayout CorridorDataOut = FCorridorLayout();;
	CorridorDataOut.StartRoomId = StartEndRoomIn.RoomA;
	CorridorDataOut.EndRoomId = StartEndRoomIn.RoomB;

	TArray<FTerrain_CellDistance> AllCorridorsPathDistances = GenerateAllCorridorsPathsDistance(CorridorDataOut.StartRoomId, CorridorDataOut.EndRoomId);
	if (AllCorridorsPathDistances.Num() == 0)
	{
		UE_LOG(TerrainGeneratorLog, Error, TEXT("FCorridorLayoutWorker::GenerateCorridorLayoutData - Could not generate path for corridor. A room has no doors."));
		SuccesOut = false;
		return FCorridorLayout();
	}

	bool PathGeneratedSuccesfully = false;
	const int32 CorridorPaths = AllCorridorsPathDistances.Num();
	int32 PathIndexOut = 0;
	for (int32 i = 0; i < CorridorPaths; i++)  //Usually this only should be run once, but if the path somehow fails, we choose another one.
	{			
		FTerrain_CellDistance SelectedPath = SelectCorridorPath(AllCorridorsPathDistances, PathIndexOut);		
		CorridorDataOut.StartCellId = SelectedPath.CellA;
		CorridorDataOut.EndCellId = SelectedPath.CellB;
		CorridorDataOut.Cells = GeneratePath(SelectedPath.CellA, SelectedPath.CellB, PathGeneratedSuccesfully);

		if (!PathGeneratedSuccesfully)
		{
			AllCorridorsPathDistances.RemoveAt(PathIndexOut);
		}
		else
		{
			break;
		}
	}

	GenerateCorridorRange(CorridorDataOut);

	SuccesOut = PathGeneratedSuccesfully;
	return  CorridorDataOut;
}

TArray<FIntPoint> FCorridorLayoutWorker::GetAllCorridorDoorsOfRoom(const FIntPoint& RoomIn) const
{
	if (!RoomsLayoutMap.Contains(RoomIn))
	{
		return TArray<FIntPoint>();
	}

	TArray<FIntPoint> Doors = TArray<FIntPoint>();
	for (const FIntPoint& cellId : RoomsLayoutMap[RoomIn].Cells)
	{
		if (!CellsLayoutMap.Contains(cellId))
		{
			continue;
		}

		if (CellsLayoutMap[cellId].Tags.HasTag(TAG_TERRAIN_CELL_LAYOUT_BORDER))
		{
			TArray<FIntPoint> AdjacentCells = UTerrainLayoutFunctionLibrary::GetAdyacentCellsOfCell(cellId);

			for (const FIntPoint AdCell : AdjacentCells)
			{
				if (CellsLayoutMap.Contains(AdCell)) //Cell is used already?
				{
					continue;
				}

				Doors.AddUnique(AdCell);
			}
		}		
	}	

	return Doors;
}

int32 FCorridorLayoutWorker::GetCellUnitDistanceBetweenCells(const FIntPoint& CellA, const FIntPoint& CellB) const
{
	return FMath::Abs(CellA.X - CellB.X) + FMath::Abs(CellA.Y - CellB.Y);
}

TArray<FTerrain_CellDistance> FCorridorLayoutWorker::GenerateAllCorridorsPathsDistance(const FIntPoint& StartRoomIn, const FIntPoint& EndRoomIn) const
{
	TArray<FIntPoint> StartRoomCorridorDoors = GetAllCorridorDoorsOfRoom(StartRoomIn);
	TArray<FIntPoint> EndRoomCorridorDoors = GetAllCorridorDoorsOfRoom(EndRoomIn);

	if (StartRoomCorridorDoors.Num() == 0 || EndRoomCorridorDoors.Num() == 0)
	{
		return TArray<FTerrain_CellDistance>(); //If one room has no doors, then the corridor cannot be made.
	}
	
	TArray<FTerrain_CellDistance> PathDistances = TArray<FTerrain_CellDistance>();
	FTerrain_CellDistance CellDistance = FTerrain_CellDistance();

	for (const FIntPoint& StartDoor : StartRoomCorridorDoors)
	{
		for (const FIntPoint& EndDoor : EndRoomCorridorDoors)
		{
			CellDistance.CellA = StartDoor;
			CellDistance.CellB = EndDoor;
			CellDistance.UnitCellDistance = GetCellUnitDistanceBetweenCells(StartDoor, EndDoor);
			PathDistances.Add(CellDistance);
		}
	}

	return PathDistances;
}

FTerrain_CellDistance FCorridorLayoutWorker::SelectCorridorPath(const TArray<FTerrain_CellDistance>& PathsDistanceIn, int32& PathIndexOut) const
{
	FTerrain_CellDistance SelectedPath = FTerrain_CellDistance();
	TArray<FTerrain_CellDistance> MinDistancePaths = TArray<FTerrain_CellDistance>();
	int32 RandomIndex = 0;
	int32 MinimunDistance = 0;

	switch (TerrainLayoutData->CorridorSelectionType)
	{
	case ETerrainGen_CorridorPathSelection::Random:
		RandomIndex = Stream.RandRange(0, PathsDistanceIn.Num() - 1);
		SelectedPath = PathsDistanceIn[RandomIndex];
		break;
	case ETerrainGen_CorridorPathSelection::Threshold:		
		MinDistancePaths = GetMinimunCellDistancePaths(PathsDistanceIn);
		MinimunDistance = MinDistancePaths[0].UnitCellDistance;
		
		MinDistancePaths = GetThresholdCellDistance(PathsDistanceIn, TerrainLayoutData->CorridorSelectionThreshold + MinimunDistance);
		
		RandomIndex = Stream.RandRange(0, MinDistancePaths.Num() - 1);
		SelectedPath = MinDistancePaths[RandomIndex];
		break;
	default:
		break;
	}

	return SelectedPath;
}

TArray<FTerrain_CellDistance> FCorridorLayoutWorker::GetMinimunCellDistancePaths(const TArray<FTerrain_CellDistance>& PathsDistanceIn) const
{
	int SmallestDistance = 10000000;
	TArray<FTerrain_CellDistance> AllSelectedPaths = TArray<FTerrain_CellDistance>();
	
	for (const FTerrain_CellDistance& Path : PathsDistanceIn)
	{
		if (Path.UnitCellDistance < SmallestDistance)
		{
			SmallestDistance = Path.UnitCellDistance;
			AllSelectedPaths.Empty();
			AllSelectedPaths.Add(Path);
		}
		else if (Path.UnitCellDistance == SmallestDistance)
		{
			AllSelectedPaths.Add(Path);
		}
	}

	return AllSelectedPaths;
}

TArray<FTerrain_CellDistance> FCorridorLayoutWorker::GetThresholdCellDistance(const TArray<FTerrain_CellDistance>& PathsDistanceIn, int32 Threshold) const
{
	TArray<FTerrain_CellDistance> AllSelectedPaths = TArray<FTerrain_CellDistance>();
	
	for (const FTerrain_CellDistance& Path : PathsDistanceIn)
	{
		if (Path.UnitCellDistance <= Threshold)
		{
			AllSelectedPaths.Add(Path);
		}
	}

	return AllSelectedPaths;
}

TArray<FIntPoint> FCorridorLayoutWorker::GeneratePath(const FIntPoint& StartCell, const FIntPoint& EndCell, bool& SuccesOut) const
{
	TArray<FPathFindingCellData> NodesTrack = TArray<FPathFindingCellData>(); //Nodes track

	if (IsNodeBlocking(EndCell))
	{
		SuccesOut = false;
		UE_LOG(TerrainGeneratorLog, Error, TEXT("FCorridorLayoutWorker::GeneratePath -  End cell of path is blocking cell. Aborted corridor generation."));
	}

	//Add start cell to open nodes
	FPathFindingCellData CurrentAdjacentNodeTrack = FPathFindingCellData(); //Use this to set first node
	CurrentAdjacentNodeTrack.Cell_ID = StartCell;
	CurrentAdjacentNodeTrack.State = EPathFindingNodeState::Open;
	NodesTrack.Add(CurrentAdjacentNodeTrack);

	TArray<FIntPoint> AdjacentCells = TArray<FIntPoint>();

	int CurrentNodeIndex = 0;

	float NewMovementCostToAdyacent = 0;
	bool FoundFirstOpenNode = false;
	bool whileEnable = true;
	int32 whilecounter = 0;

	while (whileEnable && whilecounter < 100)
	{
		whilecounter++;

		FoundFirstOpenNode = false;

		for (int i = 1; i < NodesTrack.Num(); i++)
		{
			if (NodesTrack[i].State == EPathFindingNodeState::Open)
			{
				if (!FoundFirstOpenNode)
				{
					CurrentNodeIndex = i;
					FoundFirstOpenNode = true;
				}
				else
				{
					//the node has lower f cost? or has same F cost but lower H cost
					if (NodesTrack[i].GetFCost() < NodesTrack[CurrentNodeIndex].GetFCost() || (NodesTrack[i].GetFCost() == NodesTrack[CurrentNodeIndex].GetFCost() && NodesTrack[i].HCost < NodesTrack[CurrentNodeIndex].HCost))
					{		
						CurrentNodeIndex = i;
					}
				}
			}
		}
		
		NodesTrack[CurrentNodeIndex].State = EPathFindingNodeState::Closed;
		
		if (NodesTrack[CurrentNodeIndex].Cell_ID == EndCell)
		{	
			whileEnable = false;
			SuccesOut = true;
		}
		else //did not end the while
		{
			//Get Current node adyacent cells
			AdjacentCells = UTerrainLayoutFunctionLibrary::GetAdyacentCellsOfCell(NodesTrack[CurrentNodeIndex].Cell_ID);

			for (const FIntPoint& CurrentAdyCell : AdjacentCells)
			{
				CurrentAdjacentNodeTrack = GetPathNodeTrack(CurrentAdyCell, NodesTrack);
				
				if (CurrentAdjacentNodeTrack.State == EPathFindingNodeState::Closed) continue;
				
				if(IsNodeBlocking(CurrentAdyCell)) continue; //If using path weight instead, this line should be removed. The problem is that weights are not useful in this situation since corridors are all independant etc.
				
				NewMovementCostToAdyacent = NodesTrack[CurrentNodeIndex].GCost + UTerrainLayoutFunctionLibrary::GetWorldDistanceBetweenCells(NodesTrack[CurrentNodeIndex].Cell_ID, CurrentAdyCell, CellSize);// +GetNodePathWeigth(CurrentAdyCell);

				//If the cost is lower or the current ady is not in open array
				if (NewMovementCostToAdyacent < CurrentAdjacentNodeTrack.GCost || CurrentAdjacentNodeTrack.State != EPathFindingNodeState::Open)
				{
					CurrentAdjacentNodeTrack.GCost = NewMovementCostToAdyacent;
					CurrentAdjacentNodeTrack.HCost = UTerrainLayoutFunctionLibrary::GetWorldDistanceBetweenCells(CurrentAdyCell, EndCell, CellSize);
					CurrentAdjacentNodeTrack.Parent = NodesTrack[CurrentNodeIndex].Cell_ID;
					
					CurrentAdjacentNodeTrack.State = EPathFindingNodeState::Open;
					
					SetPathNodeTrack(CurrentAdyCell, CurrentAdjacentNodeTrack, NodesTrack);
				}
			}
		}

		if (whilecounter >= 100) // Sometimes the path is invalid if attempts to go outside the level borders. We will choose another path.
		{
			whileEnable = false;
			SuccesOut = false;
			UE_LOG(TerrainGeneratorLog, Log, TEXT("FCorridorLayoutWorker::GeneratePath -  Generate Path failed reaching max While Counter: %i."), whilecounter);
		}
	}

	UE_LOG(TerrainGeneratorLog, Log, TEXT("FCorridorLayoutWorker::GeneratePath -  Generate Path While Counter: %i."), whilecounter);

	if (!SuccesOut)
	{
		return TArray<FIntPoint>();
	}
	
	return RetracePath(StartCell, EndCell, NodesTrack);
}

TArray<FIntPoint> FCorridorLayoutWorker::RetracePath(const FIntPoint& StartCell, const FIntPoint& EndCell, const TArray<FPathFindingCellData>& TrackIn) const
{
	if (TrackIn.Num() == 0)
	{
		UE_LOG(TerrainGeneratorLog, Error, TEXT("FCorridorLayoutWorker::RetracePath -  Retrace Path failed. Track is empty."));
		return TArray<FIntPoint>();
	}

	TArray<FIntPoint> Path = TArray<FIntPoint>();

	FIntPoint CurrentNode = EndCell;  //Start from the end cell and trace path backwards
	int32 whilecounter = 0;

	while (CurrentNode != StartCell && whilecounter<500)
	{
		whilecounter++;
		Path.AddUnique(CurrentNode);
		CurrentNode = GetPathNodeTrack(CurrentNode, TrackIn).Parent;
	}

	Path.AddUnique(StartCell); //Start cell is not added at the end of the while loop. I add it manually

	if (whilecounter >= 500)
	{
		UE_LOG(TerrainGeneratorLog, Error, TEXT("FCorridorLayoutWorker::RetracePath - Retraced Path failed since reaching max while counter: %i"), whilecounter);
	}
	else
	{
		UE_LOG(TerrainGeneratorLog, Log, TEXT("FCorridorLayoutWorker::RetracePath - Retraced Path with %i cells. While counter: %i"), Path.Num(), whilecounter);
	}
	
	return Path;
}

float FCorridorLayoutWorker::GetNodePathWeigth(const FIntPoint& NodeIn) const
{
	int32 NodePathingWeight = 0; //Default value, no penalty for empty cells

	//@TODO: Not using the weights just yet, may reconnect if needed
	
	////Get the cell data
	//int32 CellIndex = UTerrainLayoutFunctionLibrary::FindCellIDIndex(NodeIn, CellsData);

	//if (CellsData.IsValidIndex(CellIndex))
	//{
	//	switch (CellsData[CellIndex].Type)
	//	{
	//	case ETerrainGen_TerrainType::Normal: NodePathingWeight = RoomPathWeight;
	//		break;
	//	case ETerrainGen_TerrainType::Corridor: NodePathingWeight = CorridorPathWeight;
	//		break;
	//	case ETerrainGen_TerrainType::Blocking: NodePathingWeight = 50000; //These are walls. There is no walls at this points the cells
	//		break;
	//	default:
	//		break;
	//	}
	//}

	return NodePathingWeight;
}

bool FCorridorLayoutWorker::IsNodeBlocking(const FIntPoint& NodeIn) const
{
	bool IsBlocking = false; //Default value, no penalty for empty cells

	if (CellsLayoutMap.Contains(NodeIn))
	{
		IsBlocking = true;
	}

	return IsBlocking;
}

int32 FCorridorLayoutWorker::SetPathNodeTrack(const FIntPoint& NodeIn, const FPathFindingCellData& NewValue, TArray<FPathFindingCellData>& TrackOut) const
{
	bool UpdatedNode = false;
	int32 index = -1;

	//Try to update value of node
	for (int i = 0; i < TrackOut.Num(); i++)
	{
		if (TrackOut[i].Cell_ID == NodeIn)
		{
			TrackOut[i] = NewValue;
			UpdatedNode = true;
			index = i;
			break;
		}
	}

	//If cannot find the node, just add it.
	if (!UpdatedNode)
	{
		TrackOut.Add(NewValue);
		index = TrackOut.Num() - 1;
	}

	return index;
}

FPathFindingCellData FCorridorLayoutWorker::GetPathNodeTrack(const FIntPoint& NodeIn, const TArray<FPathFindingCellData>& TrackIn) const
{
	FPathFindingCellData TrackOut = FPathFindingCellData();
	TrackOut.Cell_ID = NodeIn; 	// If the node is not found, returns the empty struct, but must ensure the ID is correct

	for (const FPathFindingCellData& Element : TrackIn)
	{
		if (Element.Cell_ID == NodeIn)
		{
			TrackOut = Element;
			break;
		}
	}

	return TrackOut;
}

void FCorridorLayoutWorker::GenerateCorridorRange(FCorridorLayout& CorridorLayoutOut) const
{
	TArray <FIntPoint> GeneratedCells;
	GeneratedCells.Empty();

	const int32 GeneratedRange = Stream.RandRange(TerrainLayoutData->CorridorsMinRange, TerrainLayoutData->CorridorsMaxRange);

	for (const FIntPoint& cell : CorridorLayoutOut.Cells)
	{
		TArray<FIntPoint> CurrentGeneratedCells = UTerrainLayoutFunctionLibrary::GetSurroundingCellsOfCell(cell, GeneratedRange);

		for (const FIntPoint& CurrentGenCell : CurrentGeneratedCells)
		{
			if (CellsLayoutMap.Contains(CurrentGenCell))
			{
				continue;
			}

			GeneratedCells.Add(CurrentGenCell);
		}
	}	

	for (const FIntPoint& cell : GeneratedCells)
	{
		if (!CorridorLayoutOut.Cells.Contains(cell))
		{
			CorridorLayoutOut.Cells.Add(cell);
		}
	}
}
