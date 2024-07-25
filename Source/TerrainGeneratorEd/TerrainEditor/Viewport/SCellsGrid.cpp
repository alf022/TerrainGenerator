//Copyright 2020 Marchetti Alfredo. All Rights Reserved.
#include "SCellsGrid.h"
#include "Slate/SlateVectorArtInstanceData.h"
#include "Brushes/SlateColorBrush.h"

void SCellsGrid::Construct(const FArguments& InArgs)
{
	LineThickness = InArgs._LineThickness;
};

int32 SCellsGrid::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyClippingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	//Fit the most of screen without stretching
	float SizeX = (AllottedGeometry.GetLocalSize().X - ((MaxCellsX + 1) * LineThickness)) / MaxCellsX;
	float SizeY = (AllottedGeometry.GetLocalSize().Y - ((MaxCellsY + 1) * LineThickness)) / MaxCellsY;
	float MinSide = FMath::Min(SizeX, SizeY);

	//const FVector2D CellSize = FVector2D(MinSide);

	TSharedPtr<FSlateColorBrush> Brush = nullptr;
	Brush =	MakeShared<FSlateColorBrush>(FLinearColor::White);

	for (const FCellGridData& CellData : CellsData)
	{	
		FVector2D CellPosition = GetCellPosition(CellData.GridID, MinSide);
		FVector2D NextCellPosition = GetCellPosition(CellData.GridID + FIntPoint(1, 1), MinSide);

		TArray<FVector2d> Points;
		Points.Add(CellPosition);
		Points.Add(FVector2D(NextCellPosition.X, CellPosition.Y));
		Points.Add(NextCellPosition);
		Points.Add(FVector2D(CellPosition.X, NextCellPosition.Y));
		Points.Add(CellPosition);

		FSlateDrawElement::MakeLines(
			OutDrawElements,
			LayerId,
			AllottedGeometry.ToPaintGeometry(),//CellPosition, NextCellPosition - CellPosition),
			Points,
			ESlateDrawEffect::None,
			CellData.Color);		
	}

	////Could put text for rooms IDs and stuff like that, with draw element text

		//FSlateFontInfo FontInfo = FCoreStyle::Get().GetWidgetStyle<FTextBlockStyle>("NormalText").Font;
		//FString Text = "asd";
		//FSlateDrawElement::MakeText(
		//	OutDrawElements,
		//	LayerId,
		//	AllottedGeometry.ToOffsetPaintGeometry(FVector2D(50.f, 50.)), //position
		//	Text,
		//	FontInfo,
		//	ESlateDrawEffect::None,
		//	FLinearColor::Red);

	return LayerId;
}

void SCellsGrid::SetCellsData(const TArray<FCellGridData>& InCellsData)
{
	if (InCellsData.Num() <= 0)
	{
		return;
	}

	CellsData = InCellsData;
	CalculateMaxCellsSize();
	Invalidate(EInvalidateWidgetReason::Paint);
}

void SCellsGrid::CalculateMaxCellsSize()
{
	MaxCellsX = 0; 
	MaxCellsY = 0;

	for (const FCellGridData& CellData : CellsData)
	{
		if (CellData.GridID.X > MaxCellsX)
		{
			MaxCellsX = CellData.GridID.X;
		}

		if (CellData.GridID.Y > MaxCellsY)
		{
			MaxCellsY = CellData.GridID.Y;
		}
	}

	MaxCellsX++;
	MaxCellsY++;
}

FVector2D SCellsGrid::GetCellPosition(FIntPoint CellID, float MinSide) const
{
	FVector2D CellPosition = FVector2D();
	CellPosition.X = CellID.X * MinSide + (CellID.X + 1) * LineThickness;
	CellPosition.Y = CellID.Y * MinSide + (CellID.Y + 1) * LineThickness;

	return CellPosition;
}
