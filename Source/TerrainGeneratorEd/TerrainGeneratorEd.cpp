//Copyright 2020 Marchetti Alfredo. All Rights Reserved.

#pragma once
#include "TerrainGeneratorEd.h"

#include "Toolbar/TerrainGeneratorToolbarButtonStyle.h"
#include "Toolbar/TerrainGeneratorToolbarButtonCommands.h"
#include "Misc/MessageDialog.h"
#include "ToolMenus.h"

#include "StandaloneWindow/TerrainGeneratorWindowStyle.h"
#include "StandaloneWindow/TerrainGeneratorWindowCommands.h"
#include "LevelEditor.h"

#include "Widgets/Docking/SDockTab.h"
#include "TerrainEditor/SBaseTerrainEditor.h"

static const FName TerrainGeneratorWindowTabName("Terrain Generator Window");

#define LOCTEXT_NAMESPACE "FTerrainGeneratorEdModule"

void TerrainGeneratorEd::StartupModule()
{
	StartupStandaloneWindow();
	StartupToolbar();
}

void TerrainGeneratorEd::ShutdownModule()
{
	ShutdownStandaloneWindow();
	ShutdownToolbar();
}

void TerrainGeneratorEd::StartupToolbar()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	FTerrainGeneratorToolbarButtonStyle::Initialize();
	FTerrainGeneratorToolbarButtonStyle::ReloadTextures();

	FTerrainGeneratorToolbarButtonCommands::Register();

	ToolbarCommands = MakeShareable(new FUICommandList);

	ToolbarCommands->MapAction(
		FTerrainGeneratorToolbarButtonCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &TerrainGeneratorEd::TerrainGeneratorButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &TerrainGeneratorEd::RegisterToolbarMenus));
}

void TerrainGeneratorEd::ShutdownToolbar()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FTerrainGeneratorToolbarButtonStyle::Shutdown();

	FTerrainGeneratorToolbarButtonCommands::Unregister();
}

void TerrainGeneratorEd::RegisterToolbarMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	//{
	//	UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
	//	{
	//		FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
	//		Section.AddMenuEntryWithCommandList(FTerrainGeneratorToolbarButtonCommands::Get().PluginAction, ToolbarCommands);
	//	}
	//}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("PluginTools");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FTerrainGeneratorToolbarButtonCommands::Get().PluginAction));
				Entry.SetCommandList(ToolbarCommands);
			}
		}
	}
}

void TerrainGeneratorEd::StartupStandaloneWindow()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	FTerrainGeneratorWindowStyle::Initialize();
	FTerrainGeneratorWindowStyle::ReloadTextures();

	FTerrainGeneratorWindowCommands::Register();

	StandaloneWindowCommands = MakeShareable(new FUICommandList);

	StandaloneWindowCommands->MapAction(
		FTerrainGeneratorWindowCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &TerrainGeneratorEd::TerrainGeneratorButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &TerrainGeneratorEd::RegisterStandaloneWindowMenus));

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(TerrainGeneratorWindowTabName, FOnSpawnTab::CreateRaw(this, &TerrainGeneratorEd::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FTerrainGeneratorWindowTabTitle", "TerrainGeneratorWindow"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
}

void TerrainGeneratorEd::ShutdownStandaloneWindow()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FTerrainGeneratorWindowStyle::Shutdown();

	FTerrainGeneratorWindowCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(TerrainGeneratorWindowTabName);
}

void TerrainGeneratorEd::RegisterStandaloneWindowMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FTerrainGeneratorWindowCommands::Get().OpenPluginWindow, StandaloneWindowCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FTerrainGeneratorWindowCommands::Get().OpenPluginWindow));
				Entry.SetCommandList(StandaloneWindowCommands);
			}
		}
	}
}

void TerrainGeneratorEd::TerrainGeneratorButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(TerrainGeneratorWindowTabName);
}

TSharedRef<SDockTab> TerrainGeneratorEd::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	const TSharedRef<SDockTab> DockTab =
		SNew(SDockTab)
		.TabRole(ETabRole::MajorTab)
		.Label(LOCTEXT("Terrain Editor", "Terrain Editor"));

	TSharedRef<SBaseTerrainEditor> BaseTerrainEditor = SNew(SBaseTerrainEditor, DockTab, SpawnTabArgs.GetOwnerWindow());
	DockTab->SetContent(BaseTerrainEditor);

	return DockTab;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(TerrainGeneratorEd, TerrainGeneratorEd)