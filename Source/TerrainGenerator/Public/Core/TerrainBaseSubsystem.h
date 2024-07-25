//Copyright 2020 Marchetti Alfredo. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "Terrain/Types/TerrainGeneratorTypes.h"
#include "TerrainBaseSubsystem.generated.h"

class UTerrainGeneratorSubsystem;
class UTerrainThreadSubsystem;
class UTerrainData;

UCLASS(Abstract)
class TERRAINGENERATOR_API UTerrainBaseSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()

public:	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	FRandomStream& GetStream();

protected:
	UPROPERTY()
	UTerrainGeneratorSubsystem* TerrainGeneratorSubsystem;

	UPROPERTY()
	UTerrainThreadSubsystem* TerrainThreadSubsystem;

	UPROPERTY()
	UTerrainData* TerrainData;		

	UTerrainGeneratorSubsystem* GetTerrainGeneratorSubsystem();
	UTerrainThreadSubsystem* GetTerrainThreadSubsystem();

#if WITH_EDITOR
	ETerrainGen_State GetState();
	bool IsDebug();
#endif
};
