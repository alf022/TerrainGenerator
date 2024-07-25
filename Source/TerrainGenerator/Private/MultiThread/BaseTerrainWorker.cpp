//Copyright 2020 Marchetti Alfredo. All Rights Reserved.

#include "MultiThread/BaseTerrainWorker.h"

void FBaseTerrainWorker::ActivateThread(UObject* ThreadOwnerIn, int32 SeedIn)
{
	bIsThreadCompleted = false;

	ThreadOwner = ThreadOwnerIn;
	Seed = SeedIn;
	Stream = FRandomStream(Seed);
	Stream.Reset();

	Thread = FRunnableThread::Create(this, TEXT("BaseWorkerThread"));
}

void FBaseTerrainWorker::OnThreadEnd()
{

}

FBaseTerrainWorker::~FBaseTerrainWorker()
{
	if (Thread)
	{
		Thread->Kill();
		delete Thread;
	}
}

bool FBaseTerrainWorker::Init()
{	
	return true;
}
