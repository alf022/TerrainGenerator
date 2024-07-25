//Copyright 2020 Marchetti Alfredo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "TerrainGeneratorToolbarButtonStyle.h"

class FTerrainGeneratorToolbarButtonCommands : public TCommands<FTerrainGeneratorToolbarButtonCommands>
{
public:

	FTerrainGeneratorToolbarButtonCommands()
		: TCommands<FTerrainGeneratorToolbarButtonCommands>(TEXT("TerrainGeneratorToolbarButton"), NSLOCTEXT("Contexts", "TerrainGeneratorToolbarButton", "TerrainGeneratorToolbarButton Plugin"), NAME_None, FTerrainGeneratorToolbarButtonStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > PluginAction;
};
