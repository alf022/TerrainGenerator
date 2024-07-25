//Copyright 2020 Marchetti Alfredo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Core/TerrainBaseSubsystem.h"
#include "Layers/Types/TerrainLayerApplicationType.h"
#include "Layers/Types/TerrainLayerLayout.h"
#include "TerrainLayersSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FNoParamsDynamicLayersSubsystemSignature);
DECLARE_MULTICAST_DELEGATE(FNoParamsDelegateLayersSubsystemSignature);

class UTerrainLayerCollectionData;

UCLASS()
class TERRAINGENERATOR_API UTerrainLayersSubsystem : public UTerrainBaseSubsystem
{
	GENERATED_BODY()

public:
	friend class STerrainEditorViewport;
	friend class FLayerLayoutWorker;

	FNoParamsDelegateLayersSubsystemSignature OnLayersLayoutGeneratedED;
	FNoParamsDynamicLayersSubsystemSignature OnLayersLayoutGenerated;
	
	void GenerateTerrainLayersLayout(UTerrainData* InTerrainData);
	
protected:
	UPROPERTY(Transient)
	UTerrainLayerCollectionData* TerrainLayerCollectionData;
	
	UPROPERTY(Transient)
	TMap<int32, FTerrainLayerLayout> LayersLayoutsMap;

	void GenerateLayoutsInitialCells(const FLayerApplicationData& ApplicationData, TArray<FIntPoint>& CellsOut);
	TArray<FIntPoint> GetCellsByRoll(const FLayerApplicationData& ApplicationData, TArray<FIntPoint>& CellsPool);
	
	void GenerateLayersLayouts();
	void StartLayerLayoutGeneration();

	UFUNCTION()
	void OnLayerLayoutEnd();
	
	void ApplyLayers();

	int32 GetMaxLayerApplicationOrder() const;
};
