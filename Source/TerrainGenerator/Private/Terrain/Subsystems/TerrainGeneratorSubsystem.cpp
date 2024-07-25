//Copyright 2020 Marchetti Alfredo. All Rights Reserved.
#include "Terrain/Subsystems/TerrainGeneratorSubsystem.h"
#include "Layout/Subsystems/TerrainLayoutSubsystem.h"
#include "MultiThread/TerrainThreadSubsystem.h"
#include "Terrain/Data/TerrainData.h"
#include "Editor/EditorEngine.h"
#include "Logs/TerrainGeneratorLogs.h"
#include "Layers/Subsystems/TerrainLayersSubsystem.h"

void UTerrainGeneratorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

FRandomStream& UTerrainGeneratorSubsystem::GetStream()
{
	return Stream;
}

void UTerrainGeneratorSubsystem::InitializeSeed()
{
	if (!TerrainData)
	{
		return;
	}

	if (TerrainData->bRandomSeed)
	{
		Stream = FRandomStream();
		Stream.GenerateNewSeed();
	}
	else
	{
		Stream = FRandomStream(TerrainData->Seed);
		Stream.Reset();
	}
}

#if WITH_EDITOR
void UTerrainGeneratorSubsystem::EditorInitializeSeed(bool MaintainSeed)
{
	if (MaintainSeed)
	{
		Stream.Reset();
	}
	else
	{
		InitializeSeed();
	}
}
#endif

#if WITH_EDITOR

ETerrainGen_State UTerrainGeneratorSubsystem::GetState() const
{
	return State;
}

bool UTerrainGeneratorSubsystem::IsDebug() const
{
	return bIsDebug;
}

void UTerrainGeneratorSubsystem::EditorGenerateTerrain(UTerrainData* InTerrainData, ETerrainGen_State InState, bool MaintainSeed)
{
	if (!InTerrainData)
	{
		UE_LOG(TerrainGeneratorLog, Error, TEXT("UTerrainGeneratorSubsystem::GenerateTerrain - Invalid level data."));
		return;
	}

	UTerrainThreadSubsystem* TerrainThreadSubsystem = GEngine->GetEngineSubsystem<UTerrainThreadSubsystem>();
	if (!TerrainThreadSubsystem)
	{
		UE_LOG(TerrainGeneratorLog, Error, TEXT("UTerrainGeneratorSubsystem::GenerateTerrain - Invalid layout subsystem."));
		return;
	}

	TerrainThreadSubsystem->StopThreads();
	TerrainData = InTerrainData;
	State = InState;
	bIsDebug = true;
	EditorInitializeSeed(MaintainSeed);

	UTerrainLayoutSubsystem* TerrainLayoutSubsystem = GEngine->GetEngineSubsystem<UTerrainLayoutSubsystem>();
	if (!TerrainLayoutSubsystem)
	{
		UE_LOG(TerrainGeneratorLog, Error, TEXT("UTerrainGeneratorSubsystem::GenerateTerrain - Invalid layout subsystem."));
		return;
	}

	UTerrainLayersSubsystem* TerrainLayersSubsystem = GEngine->GetEngineSubsystem<UTerrainLayersSubsystem>();
	if (!TerrainLayersSubsystem)
	{
		UE_LOG(TerrainGeneratorLog, Error, TEXT("UTerrainGeneratorSubsystem::GenerateTerrain - Invalid layers subsystem."));
		return;
	}

	if (!TerrainLayoutSubsystem->OnLayoutGenerated.IsBound())
	{
		TerrainLayoutSubsystem->OnLayoutGenerated.AddDynamic(this, &UTerrainGeneratorSubsystem::OnLayoutGenerated);
	}

	if (!TerrainLayersSubsystem->OnLayersLayoutGenerated.IsBound())
	{
		TerrainLayersSubsystem->OnLayersLayoutGenerated.AddDynamic(this, &UTerrainGeneratorSubsystem::OnLayersLayoutGenerated);
	}
	
	TerrainLayoutSubsystem->GenerateTerrainLayout(TerrainData);
}
#endif

void UTerrainGeneratorSubsystem::GenerateTerrain(UTerrainData* InTerrainData)
{
	if (!InTerrainData)
	{
		UE_LOG(TerrainGeneratorLog, Error, TEXT("UTerrainGeneratorSubsystem::GenerateTerrain - Invalid level data."));
		return;
	}

#if WITH_EDITOR
	bIsDebug = false;
#endif

	UTerrainThreadSubsystem* TerrainThreadSubsystem = GEngine->GetEngineSubsystem<UTerrainThreadSubsystem>();
	if (!TerrainThreadSubsystem)
	{
		UE_LOG(TerrainGeneratorLog, Error, TEXT("UTerrainGeneratorSubsystem::GenerateTerrain - Invalid TerrainThreadSubsystem."));
		return;
	}
	TerrainThreadSubsystem->StopThreads();

	TerrainData = InTerrainData;
	InitializeSeed();

	UTerrainLayoutSubsystem* TerrainLayoutSubsystem = GEngine->GetEngineSubsystem<UTerrainLayoutSubsystem>();
	if (!TerrainLayoutSubsystem)
	{
		UE_LOG(TerrainGeneratorLog, Error, TEXT("UTerrainGeneratorSubsystem::GenerateTerrain - Invalid layout subsystem."));
		return;
	}
	
	UTerrainLayersSubsystem* TerrainLayersSubsystem = GEngine->GetEngineSubsystem<UTerrainLayersSubsystem>();
	if (!TerrainLayersSubsystem)
	{
		UE_LOG(TerrainGeneratorLog, Error, TEXT("UTerrainGeneratorSubsystem::GenerateTerrain - Invalid layers subsystem."));
		return;
	}
	
	if (!TerrainLayoutSubsystem->OnLayoutGenerated.IsBound())
	{
		TerrainLayoutSubsystem->OnLayoutGenerated.AddDynamic(this, &UTerrainGeneratorSubsystem::OnLayoutGenerated);
	}
	
	if (!TerrainLayersSubsystem->OnLayersLayoutGenerated.IsBound())
	{
		TerrainLayersSubsystem->OnLayersLayoutGenerated.AddDynamic(this, &UTerrainGeneratorSubsystem::OnLayersLayoutGenerated);
	}

	TerrainLayoutSubsystem->GenerateTerrainLayout(TerrainData);
}

void UTerrainGeneratorSubsystem::OnLayoutGenerated()
{
	UTerrainLayersSubsystem* TerrainLayersSubsystem = GEngine->GetEngineSubsystem<UTerrainLayersSubsystem>();
	if (!TerrainLayersSubsystem)
	{
		UE_LOG(TerrainGeneratorLog, Error, TEXT("UTerrainGeneratorSubsystem::OnLayoutGenerated - Invalid layers subsystem."));
		return;
	}

	TerrainLayersSubsystem->GenerateTerrainLayersLayout(TerrainData);	
}

void UTerrainGeneratorSubsystem::OnLayersLayoutGenerated()
{

}
