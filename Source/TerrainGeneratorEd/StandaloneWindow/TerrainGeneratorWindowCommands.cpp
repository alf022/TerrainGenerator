//Copyright 2020 Marchetti Alfredo. All Rights Reserved.

#include "TerrainGeneratorWindowCommands.h"

#define LOCTEXT_NAMESPACE "FTerrainGeneratorWindowModule"

void FTerrainGeneratorWindowCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "Terrain Generator", "Bring up Terrain Generator window", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
