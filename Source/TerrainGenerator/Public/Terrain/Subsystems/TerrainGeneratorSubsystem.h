//Copyright 2020 Marchetti Alfredo. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "Terrain/Types/TerrainGeneratorTypes.h"
#include "TerrainGeneratorSubsystem.generated.h"

class UTerrainData;

UCLASS()
class TERRAINGENERATOR_API UTerrainGeneratorSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	FRandomStream& GetStream();

#if WITH_EDITOR
	ETerrainGen_State GetState() const;
	bool IsDebug() const;
	void EditorGenerateTerrain(UTerrainData* InTerrainData, ETerrainGen_State InState, bool MaintainSeed = true);
#endif

	UFUNCTION(BlueprintCallable, Category = "Terrain Generation")
	void GenerateTerrain(UTerrainData* InTerrainData);

protected:	

#if WITH_EDITORONLY_DATA

	UPROPERTY(Transient)
	ETerrainGen_State State;

	UPROPERTY(Transient)
	bool bIsDebug = false;

#endif

	UPROPERTY(Transient)
	FRandomStream Stream;

	UPROPERTY(Transient)
	UTerrainData* TerrainData;
	
	void InitializeSeed();

#if WITH_EDITOR
	void EditorInitializeSeed(bool MaintainSeed);
#endif

	UFUNCTION()
	void OnLayoutGenerated();

	UFUNCTION()
	void OnLayersLayoutGenerated();
};
