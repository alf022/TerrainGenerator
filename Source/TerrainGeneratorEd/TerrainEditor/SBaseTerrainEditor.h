//Copyright 2020 Marchetti Alfredo. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class FTabManager;
class FSpawnTabArgs;
class FMenuBuilder;

//class STerrainEditorActionsTab;
class STerrainEditorViewport;

class SBaseTerrainEditor : public SCompoundWidget
{		
public:
	SLATE_BEGIN_ARGS(SBaseTerrainEditor) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const TSharedRef<SDockTab>& ConstructUnderMajorTab, const TSharedPtr<SWindow>& ConstructUnderWindow);
		
	TSharedPtr<FTabManager> GetTabManager() const { return TabManager; }
	TSharedPtr<STerrainEditorViewport> GetTerrainEditorViewport() const { return TerrainEditorViewport; }

private:
	/** Holds the tab manager that manages the front-end's tabs. */
	TSharedPtr<FTabManager> TabManager;

	/** Callback for spawning tabs. */
	TSharedRef<SDockTab> HandleTabManagerSpawnTab(const FSpawnTabArgs& Args, FName TabIdentifier);

	//TSharedPtr<STerrainEditorActionsTab> TerrainEditorActionsTab;
	TSharedPtr<STerrainEditorViewport> TerrainEditorViewport;

protected:
	/**
	 * Fills the Window menu with menu items.
	 *
	 * @param MenuBuilder The multi-box builder that should be filled with content for this pull-down menu.
	 * @param TabManager A Tab Manager from which to populate tab spawner menu items.
	 */
	static void FillWindowMenu(FMenuBuilder& MenuBuilder, const TSharedPtr<FTabManager> TabManager);
};