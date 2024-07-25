//Copyright 2020 Marchetti Alfredo. All Rights Reserved.
#include "SCellsGridMesh.h"
#include "Slate/SlateVectorArtInstanceData.h"
#include "Brushes/SlateColorBrush.h"
#include "Slate/SlateVectorArtData.h"

#define MeshPath "/TerrainGenerator/Cells/CellParticleVectorArt.CellParticleVectorArt"

void SCellsGridMesh::Construct(const FArguments& InArgs)
{
	LineThickness = InArgs._LineThickness;	
};

int32 SCellsGridMesh::OnPaint(const FPaintArgs & Args, const FGeometry & AllottedGeometry, const FSlateRect & MyClippingRect, FSlateWindowElementList & OutDrawElements, int32 LayerId, const FWidgetStyle & InWidgetStyle, bool bParentEnabled) const
{			
	FVector2D PositionOffset = AllottedGeometry.AbsoluteToLocal(AllottedGeometry.GetAbsolutePosition());
	
	// Trail
	if (TrailMeshId != -1)
	{
		FSlateInstanceBufferData PerInstaceUpdate;

		//Fit the most of screen without stretching
		float SizeX = (AllottedGeometry.GetLocalSize().X - ((MaxCellsX + 1) * LineThickness)) / MaxCellsX;
		float SizeY = (AllottedGeometry.GetLocalSize().Y - ((MaxCellsY + 1) * LineThickness)) / MaxCellsY;
		float MinSide = FMath::Min(SizeX, SizeY);

		TSharedPtr<FSlateColorBrush> Brush = nullptr;
		Brush =	MakeShared<FSlateColorBrush>(FLinearColor::White);

		for (const FCellGridData& CellData : CellsData)
		{	
			FVector2D CellPosition = FVector2D();
			CellPosition.X = CellData.GridID.X * MinSide + (CellData.GridID.X + 1) * LineThickness + MinSide / 2;
			CellPosition.Y = CellData.GridID.Y * MinSide + (CellData.GridID.Y + 1) * LineThickness + MinSide / 2;

			CellPosition = AllottedGeometry.LocalToAbsolute(CellPosition);
				
			FSlateVectorArtInstanceData CellGeneratedData;
			CellGeneratedData.SetPosition(CellPosition);
			CellGeneratedData.SetScale(MinSide / 16.f); //Particle mesh is 16.f squared pixels. Just because that is the mesh we have.
			CellGeneratedData.SetBaseAddress(MeshPackFloats_3(CellData.Color.R, CellData.Color.G, CellData.Color.B));

			PerInstaceUpdate.Add((FVector4f)CellGeneratedData.GetData());		
		}
			
		const_cast<SCellsGridMesh*>(this)->UpdatePerInstanceBuffer(TrailMeshId, PerInstaceUpdate);
	}

	return SMeshWidget::OnPaint(Args, AllottedGeometry, MyClippingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
}

void SCellsGridMesh::SetCellsData(const TArray<FCellGridData>& InCellsData)
{
	if (InCellsData.Num() <= 0)
	{
		return;
	}

	CellsData = InCellsData;	

	FSoftObjectPath Path = FSoftObjectPath();
	Path.SetPath(FPackageName::ExportTextPathToObjectPath(TEXT(MeshPath)));
	SlateVectorArtData = Cast<USlateVectorArtData>(Path.TryLoad());

	if (SlateVectorArtData && TrailMeshId == -1)
	{
		const int32 MAX_INSTANCES = InCellsData.Num();
		TrailMeshId = AddMeshWithInstancing(*SlateVectorArtData, MAX_INSTANCES);
	}

	CalculateMaxCellsSize();

	Invalidate(EInvalidateWidgetReason::Paint);
}

void SCellsGridMesh::CalculateMaxCellsSize()
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
