//Copyright 2020 Marchetti Alfredo. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "STerrainViewportTypes.h"

class SCellsGrid : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SCellsGrid) { }
	SLATE_ARGUMENT(float, LineThickness)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
		const FSlateRect& MyClippingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

	void SetCellsData(const TArray <FCellGridData>& InCellsData);

protected:
	float LineThickness = 1.f;
	int32 MaxCellsX = 0;  
	int32 MaxCellsY = 0;
	TArray <FCellGridData> CellsData;

	void CalculateMaxCellsSize();
	FVector2D GetCellPosition(FIntPoint CellID, float MinSide) const;

};
