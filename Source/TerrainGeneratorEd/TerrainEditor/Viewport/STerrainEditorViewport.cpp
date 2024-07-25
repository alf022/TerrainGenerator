//Copyright 2020 Marchetti Alfredo. All Rights Reserved.
#include "STerrainEditorViewport.h"
#include "SlateOptMacros.h"

#include "SCellsGrid.h"
#include "SCellsGridMesh.h"
#include "Widgets/SInvalidationPanel.h"

#include "GameplayTagContainer.h"
#include "Tags/TerrainTags.h"

#include "Delegates/DelegateSignatureImpl.inl"
#include "Layout/Helpers/TerrainLayoutFunctionLibrary.h"
#include "Layout/Subsystems/TerrainLayoutSubsystem.h"
#include "Layout/Types/LayoutTypes.h"

#define LOCTEXT_NAMESPACE "STerrainEditorViewport"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void STerrainEditorViewport::Construct(const FArguments& InArgs)
{
	ChildSlot
	[
		SNew(SBorder)
		.ColorAndOpacity(FLinearColor::Gray)
		.Padding(2.f)
		[
			SNew(SOverlay)
			+ SOverlay::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)			
			[			
				SAssignNew(InvalidationPanel, SInvalidationPanel)				
				[
					SAssignNew(CellsGridMesh, SCellsGridMesh)
					.LineThickness(0.f)										
				]			
			]
			+ SOverlay::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SAssignNew(InvalidationPanelMesh, SInvalidationPanel)
				[
					SAssignNew(CellsGrid, SCellsGrid)				
					.LineThickness(0.f)					
				]
			]
		]
	];

	InvalidationPanel.Get()->SetCanCache(true);
	InvalidationPanelMesh.Get()->SetCanCache(true);

	UTerrainLayoutSubsystem* TerrainLayoutSubsystem = GEngine->GetEngineSubsystem<UTerrainLayoutSubsystem>();
	if (!TerrainLayoutSubsystem)
	{
		return;
	}

	TerrainLayoutSubsystem->OnInitialLayoutGeneratedED.AddSP(this, &STerrainEditorViewport::DrawLayout);
	TerrainLayoutSubsystem->OnInitialLayoutMovementCompletedED.AddSP(this, &STerrainEditorViewport::DrawLayout);
	TerrainLayoutSubsystem->OnCorridorLayoutGeneratedED.AddSP(this, &STerrainEditorViewport::DrawLayout);
	TerrainLayoutSubsystem->OnWallsLayoutGeneratedED.AddSP(this, &STerrainEditorViewport::DrawLayout);
	TerrainLayoutSubsystem->OnLayoutGeneratedED.AddSP(this, &STerrainEditorViewport::DrawLayout);
	
	DrawLayout();
}

void STerrainEditorViewport::DrawLayout() const
{	
	UTerrainLayoutSubsystem* TerrainLayoutSubsystem = GEngine->GetEngineSubsystem<UTerrainLayoutSubsystem>();
	if (!TerrainLayoutSubsystem)
	{
		return;
	}

	TArray <FCellGridData> CellsDataGenerated;
	
	TArray <FIntPoint> CellsIDs = TArray <FIntPoint>();
	TerrainLayoutSubsystem->CellsLayoutMap.GetKeys(CellsIDs);
	const FIntPoint MinGridSize = UTerrainLayoutFunctionLibrary::GetMinGridSize(CellsIDs);
	
	for (const TPair<FIntPoint, FCellLayout>& pair : TerrainLayoutSubsystem->CellsLayoutMap)
	{				
		FCellGridData Cell = FCellGridData();
		Cell.GridID = pair.Key;

		Cell.GridID.X -= MinGridSize.X;
		Cell.GridID.Y -= MinGridSize.Y;

		if (pair.Value.Tags.HasTag(TAG_TERRAIN_CELL_LAYOUT_INITIAL) && ViewportConfiguration.bDrawInitialCell)
		{	
			Cell.Color = FLinearColor::Red;
		}
		else if (pair.Value.Tags.HasTag(TAG_TERRAIN_CELL_LAYOUT_CENTRAL) && ViewportConfiguration.bDrawCentralCell)
		{
			Cell.Color = FLinearColor(1.f, 0.2f, 0.f);
		}
		else if (pair.Value.RoomID == TerrainLayoutSubsystem->GetInitialRoom() && pair.Value.Tags.HasTag(TAG_TERRAIN_CELL_TYPE_ROOM) && ViewportConfiguration.bDrawInitialRoom)
		{
			Cell.Color = FLinearColor(.0f, 5.f, 0.f);
		}	
		else if (pair.Value.Tags.HasTag(TAG_TERRAIN_CELL_LAYOUT_BORDER) && ViewportConfiguration.bDrawRoomBorders)
		{
			Cell.Color = FLinearColor(.5f, 5.f, 0.f);
		}
		else if (pair.Value.Tags.HasTag(TAG_TERRAIN_CELL_LAYOUT_BORDERCOLLISION) && ViewportConfiguration.bDrawRoomCollision)
		{
			Cell.Color = FLinearColor::Yellow;
		}
		else if (pair.Value.Tags.HasTag(TAG_TERRAIN_CELL_LAYOUT_DOOR) && ViewportConfiguration.bDrawDoors)
		{
			Cell.Color = FLinearColor(0.f, 0.f, .1f);
		}
		else if (pair.Value.Tags.HasTag(TAG_TERRAIN_CELL_TYPE_CORRIDOR))
		{
			if (ViewportConfiguration.bDrawCorridors)
			{				
				Cell.Color = FLinearColor(0.15f, 0.15f, .15f);
			}
			else
			{
				continue;
			}			
		}	
		else if(pair.Value.Tags.HasTag(TAG_TERRAIN_CELL_TYPE_WALL))
		{
			if (ViewportConfiguration.bDrawWalls)
			{
				Cell.Color = FLinearColor(0.001f, 0.001f, .001f);
				
			}
			else
			{
				continue;
			}		
		}
		else if (ViewportConfiguration.bDrawCells)
		{
			Cell.Color = FLinearColor(0.5f, .5f, .5f);
		}

		CellsDataGenerated.Add(Cell);				
	}

	DrawViewport(CellsDataGenerated);
}

void STerrainEditorViewport::DrawViewport(const TArray<FCellGridData>& Data) const
{
	if (ViewportConfiguration.bUseGrid)
	{
		CellsGrid->SetCellsData(Data);
		CellsGrid.Get()->SetVisibility(EVisibility::SelfHitTestInvisible);
	}
	else
	{
		CellsGrid.Get()->SetVisibility(EVisibility::Collapsed);
	}

	if (ViewportConfiguration.bUseGridMesh)
	{
		CellsGridMesh->SetCellsData(Data);
		CellsGridMesh.Get()->SetVisibility(EVisibility::SelfHitTestInvisible);
	}
	else
	{
		CellsGridMesh.Get()->SetVisibility(EVisibility::Collapsed);
	}
}

void STerrainEditorViewport::SetDrawConfiguration(const FViewportConfiguration& Configuration)
{	
	ViewportConfiguration = Configuration;
	DrawLayout();
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE