//Copyright 2020 Marchetti Alfredo. All Rights Reserved.

#include "Core/TerrainBaseSubsystem.h"

#include "Terrain/Subsystems/TerrainGeneratorSubsystem.h"

void UTerrainBaseSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UTerrainBaseSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

FRandomStream& UTerrainBaseSubsystem::GetStream()
{
	if (!TerrainGeneratorSubsystem)
	{
		TerrainGeneratorSubsystem = GEngine->GetEngineSubsystem<UTerrainGeneratorSubsystem>();
	}

	check(TerrainGeneratorSubsystem);
	return TerrainGeneratorSubsystem->GetStream();
}

UTerrainGeneratorSubsystem* UTerrainBaseSubsystem::GetTerrainGeneratorSubsystem()
{
	if (!TerrainGeneratorSubsystem)
	{
		TerrainGeneratorSubsystem = GEngine->GetEngineSubsystem<UTerrainGeneratorSubsystem>();
	}

	return TerrainGeneratorSubsystem;
}

UTerrainThreadSubsystem* UTerrainBaseSubsystem::GetTerrainThreadSubsystem()
{
	if (!TerrainThreadSubsystem)
	{
		TerrainThreadSubsystem = GEngine->GetEngineSubsystem<UTerrainThreadSubsystem>();
	}

	return TerrainThreadSubsystem;
}

bool UTerrainBaseSubsystem::IsDebug()
{
	return GetTerrainGeneratorSubsystem()->IsDebug();
}

ETerrainGen_State UTerrainBaseSubsystem::GetState()
{
	return GetTerrainGeneratorSubsystem()->GetState();
}
