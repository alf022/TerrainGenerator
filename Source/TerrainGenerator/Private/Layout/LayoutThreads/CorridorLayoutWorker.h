//Copyright 2020 Marchetti Alfredo. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "MultiThread/BaseTerrainWorker.h"
#include "Layout/Types/LayoutTypes.h"
#include "Layout/Types/CorridorTypes.h"

class UTerrainLayoutData;

/* Single corridor layout worker for multithreading.*/
class TERRAINGENERATOR_API FCorridorLayoutWorker : public FBaseTerrainWorker
{
public:
	FCorridorLayoutWorker(float CellSizeIn, TArray <FTerrain_RoomDistance> StartEndRoomIn, UTerrainLayoutData* TerrainLayoutDataIn, const TMap <FIntPoint, FRoomLayout>& RoomsLayoutMapIn, const TMap <FIntPoint, FCellLayout>& CellsLayoutMapIn);

	uint32 Run() override;

	TArray <FCorridorLayout> GeneratedCorridorLayout;
		
	virtual void OnThreadEnd() override;

private:
	float CellSize;
	
	UTerrainLayoutData* TerrainLayoutData;

	TArray <FTerrain_RoomDistance> StartEndRoom;	

	TMap<FIntPoint, FCellLayout> CellsLayoutMap;

	TMap <FIntPoint, FRoomLayout> RoomsLayoutMap;

	/**
	*	Generates a sub grid layout data for a corridor, that can be populated with Terrains after.
	*	Calculates the actual cells in the grid that the corridor will follow.
	*	Must have information of the populated cells in the Terrain, to know if they are blocking or not.
	*/
	FCorridorLayout	GenerateCorridorLayout(const FTerrain_RoomDistance& StartEndRoomIn, bool& SuccesOut) const;
	
	TArray <FIntPoint> GetAllCorridorDoorsOfRoom(const FIntPoint& RoomIn) const;

	int32 GetCellUnitDistanceBetweenCells(const FIntPoint& CellA, const FIntPoint& CellB) const;
	
	TArray <FTerrain_CellDistance> GenerateAllCorridorsPathsDistance(const FIntPoint& StartRoomIn, const FIntPoint& EndRoomIn) const;

	FTerrain_CellDistance SelectCorridorPath(const TArray<FTerrain_CellDistance>& PathsDistanceIn, int32& PathIndexOut) const;
	
	TArray <FTerrain_CellDistance> GetMinimunCellDistancePaths(const TArray<FTerrain_CellDistance>& PathsDistanceIn) const;
	
	TArray <FTerrain_CellDistance> GetThresholdCellDistance(const TArray<FTerrain_CellDistance>& PathsDistanceIn, int32 Threshold = 1) const;
	
	TArray <FIntPoint> GeneratePath(const FIntPoint& StartCell, const FIntPoint& EndCell, bool& SuccesOut) const;
	
	TArray <FIntPoint> RetracePath(const FIntPoint& StartCell, const FIntPoint& EndCell, const TArray<FPathFindingCellData>& TrackIn) const;
	
	float GetNodePathWeigth(const FIntPoint& NodeIn) const;
	
	bool IsNodeBlocking(const FIntPoint& NodeIn) const;
	
	int32 SetPathNodeTrack(const FIntPoint& NodeIn, const FPathFindingCellData& NewValue, TArray<FPathFindingCellData>& TrackOut) const;
	
	FPathFindingCellData GetPathNodeTrack(const FIntPoint& NodeIn, const TArray<FPathFindingCellData>& TrackIn) const;

	void GenerateCorridorRange(FCorridorLayout& CorridorLayoutOut) const;
};
