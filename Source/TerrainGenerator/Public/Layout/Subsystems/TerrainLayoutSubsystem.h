//Copyright 2020 Marchetti Alfredo. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Core/TerrainBaseSubsystem.h"
#include "Layout/Types/CorridorTypes.h"
#include "Layout/Types/LayoutTypes.h"

#include "TerrainLayoutSubsystem.generated.h"

DECLARE_MULTICAST_DELEGATE(FNoParamsDelegateLayoutSubsystemSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FNoParamsDynamicLayoutSubsystemSignature);

class UTerrainData;
class UTerrainLayoutData;
class UTerrainGeneratorSubsystem;
class UTerrainLayersSubsystem;

UCLASS()
class TERRAINGENERATOR_API UTerrainLayoutSubsystem : public UTerrainBaseSubsystem
{
	GENERATED_BODY()

public:
	friend class STerrainEditorViewport;

	friend class FRoomLayoutWorker;
	friend class FRoomMovementWorker;
	friend class FCorridorLayoutWorker;
	friend class FWallLayoutWorker;
	friend class FBiomeMovementWorker;

	friend class UTerrainBiomeSubsystem;
	friend class UTerrainLayersSubsystem;

	FNoParamsDelegateLayoutSubsystemSignature OnInitialLayoutGeneratedED;
	FNoParamsDelegateLayoutSubsystemSignature OnInitialLayoutMovementCompletedED;
	FNoParamsDelegateLayoutSubsystemSignature OnCorridorLayoutGeneratedED;
	FNoParamsDelegateLayoutSubsystemSignature OnWallsLayoutGeneratedED;
	FNoParamsDelegateLayoutSubsystemSignature OnLayoutGeneratedED;

	FNoParamsDynamicLayoutSubsystemSignature OnInitialLayoutGenerated;
	FNoParamsDynamicLayoutSubsystemSignature OnInitialLayoutMovementCompleted;
	FNoParamsDynamicLayoutSubsystemSignature OnCorridorLayoutGenerated;
	FNoParamsDynamicLayoutSubsystemSignature OnWallsLayoutGenerated;
	FNoParamsDynamicLayoutSubsystemSignature OnLayoutGenerated;

	virtual void Deinitialize() override;
	void ClearData();
	
	FIntPoint GetInitialRoom() const;
	TMap <FIntPoint, FRoomLayout> GetRoomsLayoutMap() const;
	TMap <FIntPoint, FCellLayout> GetCellsLayoutMap() const;

	bool IsCellIDValid(const FIntPoint& InCellID) const;
	FCellLayout GetCellLayout(const FIntPoint& InCellID) const;
	FRoomLayout GetRoomLayout(const FIntPoint& InRoomID) const;

	void AppendTagsToCell(const FIntPoint& InCellID, FGameplayTagContainer Tags);
	
	FVector GetCellWorldPosition(const FIntPoint& InCellsID, FVector2D InAnchor) const;
	float GetWorldDistanceBetweenRooms(const FIntPoint& RoomA, const FIntPoint& RoomB) const;
		
	void GenerateTerrainLayout(UTerrainData* InTerrainData);

	
protected:
	UPROPERTY(Transient)
	UTerrainLayoutData* TerrainLayoutData;

	/*The initial room selected for the current layout.*/
	UPROPERTY(Transient)
	FIntPoint InitialRoom;

	UPROPERTY(Transient)
	TMap <FIntPoint, FRoomLayout> RoomsLayoutMap;

	UPROPERTY(Transient)
	TArray<FCorridorLayout> CorridorLayouts; // Just for saving data, is not really used in this process, but for layers generation
	
	UPROPERTY(Transient)
	TMap <FIntPoint, FCellLayout> CellsLayoutMap;

	void GenerateInitialRoomsLayout();	
	void StartRoomLayoutGeneration();

	UFUNCTION()
	void OnRoomLayoutEnd();

	void StartRoomMovement();
	TArray<FIntPointPair> GetRoomsCentralCells(TArray<FIntPoint>& CentralCellsOut) const; 
	TArray<FIntPoint> GetRoomInitialCollisionCells(FIntPoint RoomID) const;

	UFUNCTION()
	void OnRoomMovementEnd();

	void StartCorridorsLayoutGeneration();
	TArray<FTerrain_RoomDistance> GenerateInitialCorridorsLayoutData();
	TArray<FTerrain_RoomDistance> GetAllRoomDistanceData() const;
	TArray<FTerrain_RoomDistance> GetRoomDistancesInNearArea(const FIntPoint& StartRoom) const;

	void AddUnusedCorridors(TArray<FTerrain_RoomDistance>& UnusedCorridorsIn, TArray<FTerrain_RoomDistance>& ThreeCorridorsOut);
	
	UFUNCTION()
	void OnCorridorLayoutEnd();

	void StartWallsLayoutGeneration();

	UFUNCTION()
	void OnWallsLayoutEnd();

	void OnLayoutGenerationEnd();

	void CalculateRoomsDungeonDepth();
	void CalculateCellsLayoutDepth();
};
