//Copyright 2020 Marchetti Alfredo. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "STerrainViewportTypes.h"
#include "Widgets/SCompoundWidget.h"

class SCellsGrid;
class SCellsGridMesh;
class SInvalidationPanel;
class UTerrainGeneratorSubsystem;

struct FCellGridData;

class STerrainEditorViewport : public SCompoundWidget
{		
public:
	SLATE_BEGIN_ARGS(STerrainEditorViewport) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	
	void DrawLayout() const;

	void SetDrawConfiguration(const FViewportConfiguration& Configuration);

protected:
	
	FViewportConfiguration ViewportConfiguration;

	TSharedPtr<SCellsGrid> CellsGrid;
	TSharedPtr<SCellsGridMesh> CellsGridMesh;
	TSharedPtr<SInvalidationPanel> InvalidationPanel;
	TSharedPtr<SInvalidationPanel> InvalidationPanelMesh;

	UTerrainGeneratorSubsystem* TerrainGeneratorSubsystem = nullptr;

	void DrawViewport(const TArray<FCellGridData>& Data) const;
};