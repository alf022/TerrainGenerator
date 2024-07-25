//Copyright 2020 Marchetti Alfredo. All Rights Reserved.

#include "TerrainGeneratorToolbarButtonCommands.h"

#define LOCTEXT_NAMESPACE "FTerrainGeneratorToolbarButtonModule"

void FTerrainGeneratorToolbarButtonCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "TerrainGeneratorToolbarButton", "Terrain generator", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
