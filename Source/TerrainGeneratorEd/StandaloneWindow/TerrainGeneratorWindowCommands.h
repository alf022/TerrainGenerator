//Copyright 2020 Marchetti Alfredo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "TerrainGeneratorWindowStyle.h"

class FTerrainGeneratorWindowCommands : public TCommands<FTerrainGeneratorWindowCommands>
{
public:

	FTerrainGeneratorWindowCommands()
		: TCommands<FTerrainGeneratorWindowCommands>(TEXT("TerrainGeneratorWindow"), NSLOCTEXT("Contexts", "TerrainGeneratorWindow", "TerrainGenerator Plugin"), NAME_None, FTerrainGeneratorWindowStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};