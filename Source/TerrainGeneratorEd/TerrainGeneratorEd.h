//Copyright 2020 Marchetti Alfredo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FToolBarBuilder;
class FMenuBuilder;

class TerrainGeneratorEd : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	/** This function will be bound to Command. */
	void TerrainGeneratorButtonClicked();  //Use this for both the toolbar and the menu

protected:
	//void RegisterTerrainGenEditorWidget();

private:
	void StartupToolbar();
	void ShutdownToolbar();
	void RegisterToolbarMenus();

	void StartupStandaloneWindow();
	void ShutdownStandaloneWindow();
	void RegisterStandaloneWindowMenus();


	TSharedPtr<class FUICommandList> ToolbarCommands;
	TSharedPtr<class FUICommandList> StandaloneWindowCommands;

	TSharedRef<class SDockTab> OnSpawnPluginTab(const class FSpawnTabArgs& SpawnTabArgs);
};