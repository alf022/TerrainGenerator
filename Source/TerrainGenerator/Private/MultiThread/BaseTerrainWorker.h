//Copyright 2020 Marchetti Alfredo. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "HAL/Runnable.h"

struct FCellLayout;

class TERRAINGENERATOR_API FBaseTerrainWorker : public FRunnable
{
public:
	bool Init() override;

	virtual ~FBaseTerrainWorker() override;
	
	bool bIsThreadCompleted = false;
			
	void ActivateThread(UObject* ThreadOwnerIn, int32 SeedIn);
	virtual void OnThreadEnd();

protected:
	FRunnableThread* Thread = nullptr;
	
	int32 Seed = 0;
	
	FRandomStream Stream;
	
	UObject* ThreadOwner = nullptr;

private:
	TArray<float> StreamPool;
};
