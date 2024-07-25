//Copyright 2020 Marchetti Alfredo. All Rights Reserved.
#include "MultiThread/TerrainThreadSubsystem.h"
#include "Logs/TerrainGeneratorLogs.h"
#include "TimerManager.h"
#include "Layout/Helpers/TerrainLayoutFunctionLibrary.h"

#define ThreadCheckFrequency .05f

void UTerrainThreadSubsystem::StartThreads(UObject* ThreadOwnerIn, FRandomStream& Stream, TArray <FBaseTerrainWorker*> InActiveThreads)
{	
	if (InActiveThreads.Num() <= 0)
	{
		UE_LOG(TerrainGeneratorLog, Error, TEXT("UTerrainThreadSubsystem::StartThreads - Invalid threads."));
		return;
	}

	ActiveThreads = InActiveThreads;

	for (FBaseTerrainWorker* thread : ActiveThreads)
	{
		thread->ActivateThread(ThreadOwnerIn, UTerrainLayoutFunctionLibrary::GenerateStreamSeed(Stream));
	}

	bSetTickEnabled = true;
}

void UTerrainThreadSubsystem::StopThreads()
{
	for (int i = 0; i < ActiveThreads.Num(); i++)
	{
		if (ActiveThreads[i])
		{
			delete ActiveThreads[i];
		}
	}

	ActiveThreads.Empty();

	bSetTickEnabled = false;
}

TArray<FBaseTerrainWorker*> UTerrainThreadSubsystem::GetActiveThreads()
{
	return ActiveThreads;
}

void UTerrainThreadSubsystem::Tick(float DeltaTime)
{
	TryEndActiveThreads();
}

void UTerrainThreadSubsystem::TryEndActiveThreads()
{
	if (!AreAllThreadsCompleted())
	{
		return;
	}

	bSetTickEnabled = false;
	ClearThreads();
	OnThreadOperationEnd.Broadcast();
}

bool UTerrainThreadSubsystem::AreAllThreadsCompleted() const
{
	bool AllAreCompleted = true;

	for (int i = 0; i < ActiveThreads.Num(); i++)
	{
		if (ActiveThreads[i])
		{
			if (!ActiveThreads[i]->bIsThreadCompleted)
			{
				AllAreCompleted = false;
				break;
			}
		}
	}

	return AllAreCompleted;	
}

void UTerrainThreadSubsystem::ClearThreads()
{
	for (int i = 0; i < ActiveThreads.Num(); i++)
	{
		if (ActiveThreads[i])
		{
			ActiveThreads[i]->OnThreadEnd();
			delete ActiveThreads[i];
		}
	}

	ActiveThreads.Empty();
}
