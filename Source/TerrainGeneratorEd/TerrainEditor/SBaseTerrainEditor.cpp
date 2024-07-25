//Copyright 2020 Marchetti Alfredo. All Rights Reserved.
#include "SBaseTerrainEditor.h"
#include "SlateOptMacros.h"
#include "Widgets/Docking/SDockTab.h"
#include "Framework/Docking/TabManager.h"

#include "Viewport/STerrainEditorViewport.h"
#include "Actions/STerrainEditorActionsTab.h"

#define LOCTEXT_NAMESPACE "SBaseTerrainEditor"

static const FName TerrainEditorActionsTabId("Actions");
static const FName TerrainEditorViewportId("TerrainEditorViewport");

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SBaseTerrainEditor::Construct(const FArguments& InArgs, const TSharedRef<SDockTab>& ConstructUnderMajorTab, const TSharedPtr<SWindow>& ConstructUnderWindow)
{
	// create & initialize tab manager
	TabManager = FGlobalTabmanager::Get()->NewTabManager(ConstructUnderMajorTab);
	TSharedRef<FWorkspaceItem> AppMenuGroup = TabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("TerrainEditorMenuGroupName", "Terrain Editor"));

	TabManager->SetAllowWindowMenuBar(true);

	TabManager->RegisterTabSpawner(TerrainEditorActionsTabId, FOnSpawnTab::CreateRaw(this, &SBaseTerrainEditor::HandleTabManagerSpawnTab, TerrainEditorActionsTabId))
		.SetDisplayName(LOCTEXT("TerrainEditorActionsTabTitle", "Actions"))
		.SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "TerrainEditor.Tabs.Actions"))
		.SetGroup(AppMenuGroup);

	TabManager->RegisterTabSpawner(TerrainEditorViewportId, FOnSpawnTab::CreateRaw(this, &SBaseTerrainEditor::HandleTabManagerSpawnTab, TerrainEditorViewportId))
		.SetDisplayName(LOCTEXT("TerrainEditorViewportTitle", "Viewport"))
		.SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "TerrainEditor.Tabs.Viewport"))
		.SetGroup(AppMenuGroup);

	// create tab layout
	const TSharedRef<FTabManager::FLayout> Layout = FTabManager::NewLayout("TerrainEditorLayout")
		->AddArea
		(
			FTabManager::NewPrimaryArea()
			->SetOrientation(Orient_Horizontal)
			->Split
			(
				// Actions
				FTabManager::NewStack()
				->AddTab(TerrainEditorActionsTabId, ETabState::OpenedTab)
				//->SetHideTabWell(true)
				->SetSizeCoefficient(0.25f)
			)
			->Split
			(
				// Viewport
				FTabManager::NewStack()
				->AddTab(TerrainEditorViewportId, ETabState::OpenedTab)				
				->SetSizeCoefficient(0.75f)
				->SetForegroundTab(TerrainEditorViewportId)
			)
		);

	// create & initialize main menu
	FMenuBarBuilder MenuBarBuilder = FMenuBarBuilder(TSharedPtr<FUICommandList>());

	MenuBarBuilder.AddPullDownMenu(
		LOCTEXT("WindowMenuLabel", "Window"),
		FText::GetEmpty(),
		FNewMenuDelegate::CreateStatic(&SBaseTerrainEditor::FillWindowMenu, TabManager),
		"Window"
	);

	TSharedRef<SWidget> MenuWidget = MenuBarBuilder.MakeWidget();

	ChildSlot
		[
			TabManager->RestoreFrom(Layout, ConstructUnderWindow).ToSharedRef()
		];

	// Tell tab-manager about the multi-box for platforms with a global menu bar
	TabManager->SetMenuMultiBox(MenuBarBuilder.GetMultiBox(), MenuWidget);
}

void SBaseTerrainEditor::FillWindowMenu(FMenuBuilder& MenuBuilder, const TSharedPtr<FTabManager> TabManager)
{
	if (!TabManager.IsValid())
	{
		return;
	}
	
#if !WITH_EDITOR
	FGlobalTabmanager::Get()->PopulateTabSpawnerMenu(MenuBuilder, WorkspaceMenu::GetMenuStructure().GetStructureRoot());
#endif //!WITH_EDITOR

	TabManager->PopulateLocalTabSpawnerMenu(MenuBuilder);
}

TSharedRef<SDockTab> SBaseTerrainEditor::HandleTabManagerSpawnTab(const FSpawnTabArgs& Args, FName TabIdentifier)
{
	TSharedRef<SDockTab> DockTab = SNew(SDockTab)
								   .TabRole(ETabRole::PanelTab);
		
	if (TabIdentifier == TerrainEditorActionsTabId)
	{
		TSharedPtr<SWidget> TabWidget = SNullWidget::NullWidget;
		TSharedRef<SBaseTerrainEditor> SelfReference = StaticCastSharedRef<SBaseTerrainEditor>(AsShared());
		TabWidget = SNew(STerrainEditorActionsTab, SelfReference);
		DockTab->SetContent(TabWidget.ToSharedRef());
	}
	else if (TabIdentifier == TerrainEditorViewportId)
	{
		TerrainEditorViewport = SNew(STerrainEditorViewport);
		DockTab->SetContent(TerrainEditorViewport.ToSharedRef());
	}	
	
	return DockTab;
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE