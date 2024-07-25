//Copyright 2020 Marchetti Alfredo. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "Tickable.h"
#include "MultiThread/BaseTerrainWorker.h"
#include "TerrainThreadSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FNoParamsDelegateThreadSubsystemSignature);

UCLASS()
class TERRAINGENERATOR_API UTerrainThreadSubsystem : public UEngineSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FNoParamsDelegateThreadSubsystemSignature OnThreadOperationEnd;

	TArray <FBaseTerrainWorker*> GetActiveThreads();

	void StartThreads(UObject* ThreadOwnerIn, FRandomStream& Stream, TArray <FBaseTerrainWorker*> InActiveThreads);
	void StopThreads();

	virtual bool IsTickableInEditor() const override
	{
		return true;
	}

	virtual bool IsAllowedToTick() const override
	{ 
		return bSetTickEnabled;
	}
	
	virtual TStatId GetStatId() const override
	{
		return TStatId();
	}

	virtual void Tick(float DeltaTime) override;
	
protected:
	UPROPERTY()
	bool bSetTickEnabled = false;
	
	UPROPERTY()
	FTimerHandle ThreadCheckTimer = FTimerHandle();

	TArray <FBaseTerrainWorker*> ActiveThreads;

	void TryEndActiveThreads();
	bool AreAllThreadsCompleted() const;
	void ClearThreads();
};
