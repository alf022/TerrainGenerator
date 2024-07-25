//Copyright 2020 Marchetti Alfredo. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "MultiThread/BaseTerrainWorker.h"
#include "Layout/Types/LayoutTypes.h"

class UTerrainLayoutData;

/* Room layout worker for multithreading.*/
class TERRAINGENERATOR_API FRoomLayoutWorker : public FBaseTerrainWorker
{
public:
	
	FRoomLayoutWorker(UTerrainLayoutData* TerrainLayoutDataIn, const TMap<FIntPoint, FRoomLayout>& RoomsLayoutMapIn);

	uint32 Run() override; // Main data processing happens here
		
	virtual void OnThreadEnd() override;

private:
	UTerrainLayoutData* TerrainLayoutData;
	
	TMap<FIntPoint, FRoomLayout> RoomsLayoutMap;

	void GenerateRoomsLayout();
};
