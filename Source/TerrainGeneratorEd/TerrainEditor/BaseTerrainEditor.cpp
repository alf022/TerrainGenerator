//Copyright 2020 Marchetti Alfredo. All Rights Reserved.

#include "BaseTerrainEditor.h"
#include "SBaseTerrainEditor.h"

#define LOCTEXT_NAMESPACE "BaseTerrainEditorWidget"

TSharedRef<SWidget> UBaseTerrainEditor::RebuildWidget()
{
	return SNew(SBox);
		//SNew(SBaseTerrainEditor);
		//.World(this); //this argument is not needed
}

const FText UBaseTerrainEditor::GetPaletteCategory()
{
	return LOCTEXT("PaletteCategory", "TerrainEditor");
}

#undef LOCTEXT_NAMESPACE