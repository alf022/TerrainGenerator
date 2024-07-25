//Copyright 2020 Marchetti Alfredo. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Layers/Types/TerrainLayerLayout.h"
#include "MultiThread/BaseTerrainWorker.h"


class UTerrainLayerLayoutData;

/* Layer layout worker for multithreading.*/
class TERRAINGENERATOR_API FLayerLayoutWorker : public FBaseTerrainWorker
{
public:

	FLayerLayoutWorker(const TMap<int32, FTerrainLayerLayout>& LayersLayoutMapIn);

	uint32 Run() override;
		
	virtual void OnThreadEnd() override;

private:
	
	TMap<int32, FTerrainLayerLayout> LayersLayoutMap;

	void GenerateLayersLayout();
};
