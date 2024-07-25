//Copyright 2020 Marchetti Alfredo. All Rights Reserved.
#pragma once

#include "TerrainGeneratorTypes.generated.h"

#if WITH_EDITOR

UENUM()
enum class ETerrainGen_State : uint8
{
	InitialLayout,
	InitialLayoutMovement,
	CorridorsLayout,
	WallsLayout,
	LayersLayout,	
	ETGS_MAX
};
#endif
