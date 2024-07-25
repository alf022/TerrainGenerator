//Copyright 2020 Marchetti Alfredo. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "MultiThread/BaseTerrainWorker.h"
#include "Layout/Types/LayoutTypes.h"

class UTerrainLayoutData;
class UTerrainLayoutSubsystem;

class TERRAINGENERATOR_API FRoomMovementWorker : public FBaseTerrainWorker
{
public:
	
	FRoomMovementWorker(FIntPoint InitialRoomIn, const TArray<FIntPoint>& BorderCellsIn, const TArray<FIntPointPair>& RoomCentralCellsPairsIn, UTerrainLayoutData* TerrainLayoutDataIn);

	uint32 Run() override; // Main data processing happens here
		
	virtual void OnThreadEnd() override;

private:
	FIntPoint InitialRoom;
	
	TArray<FIntPoint> BorderCells;
	
	TArray<FIntPointPair> RoomCentralCellsPairs;
	
	UTerrainLayoutData* TerrainLayoutData;
	
	TMap <FIntPoint, FCellLayout> GeneratedCellsLayoutMap;
	
	UTerrainLayoutSubsystem* TerrainLayoutSubsystem;

	void AdjustRoomsLocation();
	void MoveRoom(const FIntPoint& RoomID, const TArray<FIntPoint>& InBorderCells);
	void OffsetLayout(FIntPoint InLayout, FIntPoint InOffset);
	void GenerateRoomCellsLayout(const FIntPoint& RoomID, TArray<FIntPoint>& BorderCellsOut);
	bool IsBorderCell(FIntPoint Cell, TArray<FIntPoint> Cells) const;
};
