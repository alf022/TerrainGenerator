//Copyright 2020 Marchetti Alfredo. All Rights Reserved.
#include "Layout/Data/TerrainBaseLayoutData.h"
#include "Layout/Data/TerrainLayoutLayerData.h"
#include "Layout/Helpers/TerrainLayoutFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Misc/DataValidation.h"

#define LOCTEXT_NAMESPACE "TerrainBaseLayoutData"

int32 UTerrainBaseLayoutData::GetMaxCells() const
{
	int32 cells = 0;
	for (const UTerrainLayoutLayerData* layer : LayoutLayers)
	{	
		if (layer)
		{
			cells += layer->LayoutMaxCells;
		}	
	}
	return FMath::Max(cells, 1);
}

int32 UTerrainBaseLayoutData::GetQuadSize() const
{
	float QuadsPerSide = UKismetMathLibrary::Sqrt(GetMaxCells());	
	int32 MinQuadSize = UKismetMathLibrary::FCeil(QuadsPerSide);
	return ExtraQuadSizePercent * MinQuadSize + MinQuadSize;
}

TArray<FIntPoint> UTerrainBaseLayoutData::GenerateLayout(FIntPoint& InitialCellOut, FRandomStream& Stream, FIntPoint LayoutOffset, const TArray<FIntPoint>& CellBounds) const
{
	int32 LocalQuadSize = GetQuadSize();

	float MinCellX = LayoutOffset.X;
	float MaxCellX = LayoutOffset.X + LocalQuadSize - 1;
	float MinCellY = LayoutOffset.Y;
	float MaxCellY = LayoutOffset.Y + LocalQuadSize - 1;

	FIntPoint InitialCell = FIntPoint();
	if (bRandomizeInitialCell)
	{
		if (InitialCellMaxDeviationFromCenterPercent >= 1.f)
		{
			InitialCell = { Stream.RandRange(MinCellX, MaxCellX), Stream.RandRange(MinCellY, MaxCellY) };
		}
		else
		{
			InitialCell = { FMath::TruncToInt32((MinCellX + MaxCellX) / 2), FMath::TruncToInt32((MinCellY + MaxCellY) / 2) };

			int32 DeviationSize = (LocalQuadSize / 2) * InitialCellMaxDeviationFromCenterPercent;
			int32 Dev_X = Stream.RandRange(-DeviationSize, DeviationSize);
			int32 Dev_Y = Stream.RandRange(-DeviationSize, DeviationSize);

			InitialCell += FIntPoint(Dev_X, Dev_Y);
		}
	}
	else
	{	
		InitialCell.X = UKismetMathLibrary::MapRangeClamped(InitialCellAnchor.X, 0.f, 1.f, MinCellX, MaxCellX);
		InitialCell.Y = UKismetMathLibrary::MapRangeClamped(InitialCellAnchor.Y, 0.f, 1.f, MinCellY, MaxCellY);		
	}

	FLayoutInputData LayoutInputData = FLayoutInputData();
	LayoutInputData.Cells.Add(InitialCell);
	LayoutInputData.Bounds_Min_X = MinCellX;
	LayoutInputData.Bounds_Max_X = MaxCellX;
	LayoutInputData.Bounds_Min_Y = MinCellY;
	LayoutInputData.Bounds_Max_Y = MaxCellY;
	LayoutInputData.CellsBounds = CellBounds;

	if (CellBounds.Num() <= 0)
	{
		LayoutInputData.ToGenerateCellsScale = 1.f;
	}
	else
	{
		LayoutInputData.ToGenerateCellsScale = (CellBounds.Num() * GetMaxCells()) / (LocalQuadSize * LocalQuadSize);
	}

	for (int32 i = 0; i < LayoutLayers.Num(); i++)
	{
		LayoutInputData.LayerIndex = i;
		LayoutLayers[i]->ApplyLayer(Stream, LayoutInputData);
	}

	InitialCellOut = InitialCell;
	return LayoutInputData.Cells;
}

#if WITH_EDITOR
EDataValidationResult UTerrainBaseLayoutData::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);

	int32 EntryIndex = 0;
	for (UTerrainLayoutLayerData* layer : LayoutLayers)
	{
		if (!layer)
		{
			Result = EDataValidationResult::Invalid;
			Context.AddError(FText::Format(LOCTEXT("layerInvalid", "Null entry at index {0} in LayoutLayers"), FText::AsNumber(EntryIndex)));
		}

		++EntryIndex;
	}

	return Result;
}
#endif

#undef LOCTEXT_NAMESPACE
