//Copyright 2020 Marchetti Alfredo. All Rights Reserved.

#include "Layout/LayoutThreads/RoomMovementWorker.h"
#include "Layout/Data/TerrainLayoutData.h"
#include "Logs/TerrainGeneratorLogs.h"
#include "Layout/Helpers/TerrainLayoutFunctionLibrary.h"
#include "Layout/Subsystems/TerrainLayoutSubsystem.h"
#include "Tags/TerrainTags.h"

#pragma region Main Thread Code

// This code will be run on the thread that invoked this thread (i.e. game thread)
FRoomMovementWorker::FRoomMovementWorker(FIntPoint InitialRoomIn, const TArray<FIntPoint>& BorderCellsIn, const TArray<FIntPointPair>& RoomCentralCellsPairsIn, UTerrainLayoutData* TerrainLayoutDataIn)
{
	InitialRoom = InitialRoomIn;
	BorderCells = BorderCellsIn;
	RoomCentralCellsPairs = RoomCentralCellsPairsIn;
	TerrainLayoutData = TerrainLayoutDataIn;
}

uint32 FRoomMovementWorker::Run()
{
	TerrainLayoutSubsystem = Cast<UTerrainLayoutSubsystem>(ThreadOwner);
	if (!TerrainLayoutSubsystem)
	{
		bIsThreadCompleted = true;
		return 0;
	}

	AdjustRoomsLocation();
		
	bIsThreadCompleted = true;
	return 0;
}

void FRoomMovementWorker::OnThreadEnd()
{
	for (const TPair<FIntPoint, FCellLayout>& pair : GeneratedCellsLayoutMap)
	{
		if (!TerrainLayoutSubsystem->CellsLayoutMap.Contains(pair.Key))
		{
			TerrainLayoutSubsystem->CellsLayoutMap.Add(pair);
		}
	}
}

void FRoomMovementWorker::AdjustRoomsLocation()
{
	TArray<FIntPoint> BorderCellsLocal = BorderCells;
	
	for (const FIntPointPair& RoomAndCellID : RoomCentralCellsPairs)
	{
		if (RoomAndCellID.PointA != InitialRoom)
		{
			MoveRoom(RoomAndCellID.PointA, BorderCellsLocal);
		}

		GenerateRoomCellsLayout(RoomAndCellID.PointA, BorderCellsLocal);
	}
}

void FRoomMovementWorker::MoveRoom(const FIntPoint& RoomID, const TArray<FIntPoint>& InBorderCells)
{
	if (!TerrainLayoutSubsystem->RoomsLayoutMap.Contains(RoomID))
	{
		return;
	}

	int32 MinRoomDistanceRolled = Stream.RandRange(TerrainLayoutData->MinRoomDistance, TerrainLayoutData->MaxRoomDistance);

	FIntPoint Offset = TerrainLayoutSubsystem->RoomsLayoutMap[InitialRoom].CentralCell - TerrainLayoutSubsystem->RoomsLayoutMap[RoomID].CentralCell;

	float OffsetMax = FMath::Max(FMath::Abs(Offset.X), FMath::Abs(Offset.Y));

	FVector2D UnitaryOffset = FVector2D::ZeroVector;
	UnitaryOffset.X = (TerrainLayoutSubsystem->RoomsLayoutMap[RoomID].CentralCell.X - TerrainLayoutSubsystem->RoomsLayoutMap[InitialRoom].CentralCell.X) / OffsetMax;
	UnitaryOffset.Y = (TerrainLayoutSubsystem->RoomsLayoutMap[RoomID].CentralCell.Y - TerrainLayoutSubsystem->RoomsLayoutMap[InitialRoom].CentralCell.Y) / OffsetMax;

	for (int32 i = 1; i <= OffsetMax; i++)
	{
		FVector2D CurrentUnitary = UnitaryOffset * i;
		FIntPoint CurrentUnitaryPoint = FIntPoint();
		CurrentUnitaryPoint.X = FMath::RoundHalfToEven(CurrentUnitary.X);
		CurrentUnitaryPoint.Y = FMath::RoundHalfToEven(CurrentUnitary.Y);

		FIntPoint CurrentOffset = Offset + CurrentUnitaryPoint;

		TArray <FIntPoint> OffsetedRoomCells = TerrainLayoutSubsystem->RoomsLayoutMap[RoomID].Cells;
		for (FIntPoint& cellID : OffsetedRoomCells)
		{
			cellID += CurrentOffset;
		}

		float Distance = UTerrainLayoutFunctionLibrary::GetMinimunCellDistanceBetweenCells(OffsetedRoomCells, InBorderCells);
		//Another way of distance calculation using central cells can be MUCH FASTER but less accurate

		bool bIsDistanceValid = (Distance >= MinRoomDistanceRolled && Distance <= TerrainLayoutData->MaxRoomDistance);

		if (bIsDistanceValid || Distance > TerrainLayoutData->MaxRoomDistance)  //second condition is the better offset without going too far
		{
			OffsetLayout(RoomID, CurrentOffset);
			break;
		}
	}
}

void FRoomMovementWorker::OffsetLayout(FIntPoint InLayout, FIntPoint InOffset)
{
	if (!TerrainLayoutSubsystem->RoomsLayoutMap.Contains(InLayout))
	{
		UE_LOG(TerrainGeneratorLog, Error, TEXT("FRoomMovementWorker::OffsetLayout - Invalid layout ID."));
		return;
	}

	TerrainLayoutSubsystem->RoomsLayoutMap[InLayout].InitialCell += InOffset;
	TerrainLayoutSubsystem->RoomsLayoutMap[InLayout].CentralCell += InOffset;

	for (FIntPoint& cellID : TerrainLayoutSubsystem->RoomsLayoutMap[InLayout].Cells)
	{
		cellID += InOffset;
	}
}

void FRoomMovementWorker::GenerateRoomCellsLayout(const FIntPoint& RoomID, TArray<FIntPoint>& BorderCellsOut)
{
	if (!TerrainLayoutSubsystem->RoomsLayoutMap.Contains(RoomID))
	{
		UE_LOG(TerrainGeneratorLog, Error, TEXT("FRoomMovementWorker::GenerateRoomCellsLayout - Invalid room ID."));
		return;
	}

	for (const FIntPoint& cellID : TerrainLayoutSubsystem->RoomsLayoutMap[RoomID].Cells)
	{
		FCellLayout CellLayout = FCellLayout();
		CellLayout.RoomID = RoomID;
		CellLayout.CellID = cellID;

		CellLayout.Tags.AddTag(TAG_TERRAIN_CELL_TYPE_ROOM);

		if (TerrainLayoutSubsystem->RoomsLayoutMap[RoomID].InitialCell == cellID)
		{
			CellLayout.Tags.AddTag(TAG_TERRAIN_CELL_LAYOUT_INITIAL);
		}

		if (TerrainLayoutSubsystem->RoomsLayoutMap[RoomID].CentralCell == cellID)
		{
			CellLayout.Tags.AddTag(TAG_TERRAIN_CELL_LAYOUT_CENTRAL);
		}

		if (IsBorderCell(cellID, TerrainLayoutSubsystem->RoomsLayoutMap[RoomID].Cells))
		{
			CellLayout.Tags.AddTag(TAG_TERRAIN_CELL_LAYOUT_BORDER);

			float PrecisionRoll = Stream.FRand();
			if (PrecisionRoll <= TerrainLayoutData->RoomSeparationPrecision)
			{
				if ((cellID.X % TerrainLayoutData->RoomSeparationPatternIndex && !(cellID.Y % TerrainLayoutData->RoomSeparationPatternIndex)) ||
					(!(cellID.X % TerrainLayoutData->RoomSeparationPatternIndex) && (cellID.Y % TerrainLayoutData->RoomSeparationPatternIndex)))
				{
					CellLayout.Tags.AddTag(TAG_TERRAIN_CELL_LAYOUT_BORDERCOLLISION);
					BorderCellsOut.Add(cellID);
				}
			}
		}

		GeneratedCellsLayoutMap.Add(cellID, CellLayout);
	}
}

bool FRoomMovementWorker::IsBorderCell(FIntPoint Cell, TArray<FIntPoint> Cells) const
{
	TArray <FIntPoint> SurroundingCells = UTerrainLayoutFunctionLibrary::GetAdyacentCellsOfCell(Cell);
	bool IsBorder = false;

	for (const FIntPoint CellId : SurroundingCells)
	{
		if (!Cells.Contains(CellId) && CellId.X >= 0 && CellId.Y >= 0)
		{
			IsBorder = true;
			break;
		}
	}

	return IsBorder;
}
