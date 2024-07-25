//Copyright 2020 Marchetti Alfredo. All Rights Reserved.
#include "Layout/Data/TerrainLayoutLayerData.h"
#include "Logs/TerrainGeneratorLogs.h"
#include "Layout/LayoutObjects/BaseLayoutObject.h"
#include "Layout/Helpers/TerrainLayoutFunctionLibrary.h"
#include "Misc/DataValidation.h"

#define LOCTEXT_NAMESPACE "TerrainLayoutLayerData"

void UTerrainLayoutLayerData::ApplyLayer(FRandomStream& Stream, FLayoutInputData& LayoutInputData)
{			
	//Add data for the layout object
	LayoutInputData.MinCells = LayoutMinCells;
	LayoutInputData.MaxCells = LayoutMaxCells;

	for (int32 i = 0; i < LayerApplicationTimes; i++)
	{
		int32 SelectedLayoutObjectIndex = Stream.RandRange(0, LayoutObjects.Num() - 1);

		if (!LayoutObjects.IsValidIndex(SelectedLayoutObjectIndex))
		{
			UE_LOG(TerrainGeneratorLog, Error, TEXT("UTerrainLayoutLayerData::ApplyLayer - Invalid layout index. Failed to generate cells for the Layout. Set a valid object in the TerrainRoomLayoutData used."));
			continue;
		}

		if (!LayoutObjects[SelectedLayoutObjectIndex])
		{
			UE_LOG(TerrainGeneratorLog, Error, TEXT("UTerrainLayoutLayerData::ApplyLayer - Invalid layout object. Failed to generate cells for the Layout. Set a valid object in the TerrainRoomLayoutData used."));
			continue;
		}

		LayoutInputData.LayerApplicationIndex = i;
		LayoutObjects[SelectedLayoutObjectIndex]->GenerateLayout(UTerrainLayoutFunctionLibrary::GenerateStreamSeed(Stream), LayoutInputData);
	}	
}

#if WITH_EDITOR
EDataValidationResult UTerrainLayoutLayerData::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);

	int32 EntryIndex = 0;
	for (UBaseLayoutObject* layoutlayer : LayoutObjects)
	{
		if (!layoutlayer)
		{
			Result = EDataValidationResult::Invalid;
			Context.AddError(FText::Format(LOCTEXT("layoutlayerInvalid", "Null entry at index {0} in LayoutObjects"), FText::AsNumber(EntryIndex)));
		}

		++EntryIndex;
	}

	return Result;
}
#endif

#undef LOCTEXT_NAMESPACE
