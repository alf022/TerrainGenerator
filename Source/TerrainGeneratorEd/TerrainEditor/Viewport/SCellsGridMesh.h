//Copyright 2020 Marchetti Alfredo. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Slate/SMeshWidget.h"
#include "STerrainViewportTypes.h"

class SCellsGridMesh : public SMeshWidget
{
public:
	SLATE_BEGIN_ARGS(SCellsGridMesh) { }
	SLATE_ARGUMENT(float, LineThickness)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
		const FSlateRect& MyClippingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

	void SetCellsData(const TArray <FCellGridData>& InCellsData);

protected:
	USlateVectorArtData* SlateVectorArtData;
	int32 TrailMeshId = -1;

	float LineThickness = 1.f;
	int32 MaxCellsX = 0;  
	int32 MaxCellsY = 0;
	TArray <FCellGridData> CellsData;

	void CalculateMaxCellsSize();

	FORCEINLINE float MeshPackFloats_3(const float X, const float Y, const float Z) const
	{
		const uint8 XInt = X * 255.0f;
		const uint8 YInt = Y * 255.0f;
		const uint8 ZInt = Z * 255.0f;

		const uint32 PackedInt = (XInt << 16) | (YInt << 8) | ZInt;
		return (float)(((double)PackedInt) / ((double)(1 << 24)));
	}
};
