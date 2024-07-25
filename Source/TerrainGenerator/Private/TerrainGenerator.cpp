//Copyright 2020 Marchetti Alfredo. All Rights Reserved.

#include "TerrainGenerator.h"

#define LOCTEXT_NAMESPACE "FTerrainGeneratorModule"

void FTerrainGeneratorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FTerrainGeneratorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FTerrainGeneratorModule, TerrainGenerator)