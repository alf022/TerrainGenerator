//Copyright 2020 Marchetti Alfredo. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "MultiThread/BaseTerrainWorker.h"
#include "Layout/Types/LayoutTypes.h"

class UTerrainLayoutData;

/* Wall layout worker for multithreading.*/
class TERRAINGENERATOR_API FWallLayoutWorker : public FBaseTerrainWorker
{
public:
	
	FWallLayoutWorker(UTerrainLayoutData* TerrainLayoutDataIn, const TMap <FIntPoint, FCellLayout>& CellsLayoutMapIn);

	uint32 Run() override; // Main data processing happens here
		
	virtual void OnThreadEnd() override;

private:
	UTerrainLayoutData* TerrainLayoutData;
	
	TMap<FIntPoint, FCellLayout> CellsLayoutMap;
	
	TMap <FIntPoint, FCellLayout> GeneratedWallCells;

	void GenerateWallsLayout();
};
