//Copyright 2020 Marchetti Alfredo. All Rights Reserved.
#include "Layout/Helpers/TerrainLayoutFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Layout/Types/CorridorTypes.h"

int32 UTerrainLayoutFunctionLibrary::GenerateStreamSeed(FRandomStream& Stream)
{
	return Stream.RandRange(-999999999, 999999999);
}

FIntPoint UTerrainLayoutFunctionLibrary::GetIDInAnchor(const TArray<FIntPoint>& IDs, FVector2D Anchor) 
{
	if (IDs.Num() <= 0)
	{
		return FIntPoint();
	}

	const FIntPoint MaxGridSize = GetMaxGridSize(IDs);
	const FIntPoint MinGridSize = GetMinGridSize(IDs);

	FIntPoint PerfectCell = FIntPoint();
	PerfectCell.X = UKismetMathLibrary::MapRangeClamped(Anchor.X, 0.f, 1.f, MinGridSize.X, MaxGridSize.X);
	PerfectCell.Y = UKismetMathLibrary::MapRangeClamped(Anchor.Y, 0.f, 1.f, MinGridSize.Y, MaxGridSize.Y);

	const FIntPoint QuadSize = (MaxGridSize + MinGridSize) / 2;

	return GetClosesIDToID(IDs, PerfectCell, FMath::Max(QuadSize.X, QuadSize.Y));
	
	//---------------------------Slower method for the function
		
	//TArray<FIntPoint> localIDs = IDs;

	//FIntPoint MaxGridSize = UTerrainLayoutFunctionLibrary::GetMaxGridSize(localIDs);
	//FIntPoint MinGridSize = UTerrainLayoutFunctionLibrary::GetMinGridSize(localIDs);

	//FIntPoint PerfectCell = FIntPoint();
	//PerfectCell.X = UKismetMathLibrary::MapRangeClamped(Anchor.X, 0.f, 1.f, MinGridSize.X, MaxGridSize.X);
	//PerfectCell.Y = UKismetMathLibrary::MapRangeClamped(Anchor.Y, 0.f, 1.f, MinGridSize.Y, MaxGridSize.Y);

	////@TODO: this and next function can be optimized hard.
	////We can optimize this function, we could just look close to the anchor knowing the size
	////We stand on the cell id perfect and loop around growing untill we find a contained ID, super fast compared to it

	//return GetClosesIDToID(IDs, PerfectCell);
}

FIntPoint UTerrainLayoutFunctionLibrary::GetClosesIDToID(const TArray<FIntPoint>& IDs, const FIntPoint ID, const int32 MaxDistance)
{
	if (IDs.Contains(ID))
	{
		return ID;
	}

	FIntPoint CellAtRange = FIntPoint();
	int32 Distance = 0;
	
	while (Distance <= MaxDistance)
	{
		Distance++;
		bool bSucces = FindFirstCellAtRange(ID, IDs, CellAtRange, Distance);
		
		if (bSucces)
		{
			return CellAtRange;
		}
	}

	return IDs[0]; 
	
	//---------------------------Slower method for the function

	/*
	FIntPoint PerfectCell = ID;

	TArray<FIntPoint> localIDs = IDs;
	Algo::Sort(localIDs, [&PerfectCell](FIntPoint A, FIntPoint B)
		{
			int32 ADistance = GetCellDistanceBetweenCells(PerfectCell, A);
			int32 BDistance = GetCellDistanceBetweenCells(PerfectCell, B);

			return ADistance < BDistance;
		});

	if (localIDs.IsValidIndex(0))
	{
		return localIDs[0];
	}*/	
}

TArray<FIntPoint> UTerrainLayoutFunctionLibrary::GetSurroundingCellsOfCell(FIntPoint Cell, int32 Range)
{
	if (Range <= 0)
	{
		return TArray<FIntPoint>();
	}

	const int32 x_min = Cell.X - Range;
	const int32 x_max = Cell.X + Range;
	const int32 y_min = Cell.Y - Range;
	const int32 y_max = Cell.Y + Range;

	TArray <FIntPoint> SurroundingCells = TArray <FIntPoint>();
	for (int32 i = x_min; i <= x_max; i++)
	{
		for (int32 j = y_min; j <= y_max; j++)
		{
			SurroundingCells.Add(FIntPoint(i, j));			
		}
	}

	return SurroundingCells;
}

bool UTerrainLayoutFunctionLibrary::FindFirstCellAtRange(FIntPoint StartCell, const TArray <FIntPoint>& Cells, FIntPoint& CellAtRangeOut, int32 Range)
{
	if (Cells.Contains(StartCell))
	{
		CellAtRangeOut = StartCell;
		return true;
	}

	if (Range <= 0)
	{
		return false;
	}

	int32 x_min = StartCell.X - Range;
	int32 x_max = StartCell.X + Range;
	int32 y_min = StartCell.Y - Range;
	int32 y_max = StartCell.Y + Range;

	FIntPoint CurrentCell = FIntPoint(x_min, y_min);
	for (int32 i = x_min; i <= x_max; i++)  //Test Up border
	{
		CurrentCell.X = i;
		if (Cells.Contains(CurrentCell))
		{
			CellAtRangeOut = CurrentCell;
			return true;
		}
	}

	CurrentCell = FIntPoint(x_min, y_max);
	for (int32 i = x_min; i <= x_max; i++)  //Test down border
	{
		CurrentCell.X = i;
		if (Cells.Contains(CurrentCell))
		{
			CellAtRangeOut = CurrentCell;
			return true;
		}
	}

	CurrentCell = FIntPoint(x_min, y_min);
	for (int32 i = y_min; i <= y_max; i++)  //Test Left border
	{
		CurrentCell.Y = i;
		if (Cells.Contains(CurrentCell))
		{
			CellAtRangeOut = CurrentCell;
			return true;
		}
	}

	CurrentCell = FIntPoint(x_max, y_min);
	for (int32 i = y_min; i <= y_max; i++)  //Test Right border
	{
		CurrentCell.Y = i;
		if (Cells.Contains(CurrentCell))
		{
			CellAtRangeOut = CurrentCell;
			return true;
		}
	}

	/*for (int32 i = x_min; i <= x_max; i++)
	{
		for (int32 j = y_min; j <= y_max; j++)
		{
			if (i >= 0 && j >= 0)
			{
				if (i == x_min || i == x_max || j == y_min || j == y_max)
				{
					if (Cells.Contains(FIntPoint(i, j)))
					{
						CellAtRangeOut = FIntPoint(i, j);			
						return true;
					}
				}
			}
		}
	}*/

	return false;
}

TArray<FIntPoint> UTerrainLayoutFunctionLibrary::GetCellsOfCellAtRange(FIntPoint Cell, int32 Range)
{
	if (Range <= 0)
	{
		return TArray<FIntPoint>();
	}

	const int32 x_min = Cell.X - Range;
	const int32 x_max = Cell.X + Range;
	const int32 y_min = Cell.Y - Range;
	const int32 y_max = Cell.Y + Range;

	TArray <FIntPoint> SurroundingCells = TArray <FIntPoint>();
	for (int32 i = x_min; i <= x_max; i++)
	{
		for (int32 j = y_min; j <= y_max; j++)
		{			
			if (i == x_min || i == x_max || j == y_min || j == y_max)
			{
				SurroundingCells.Add(FIntPoint(i, j));
			}	
		}
	}

	return SurroundingCells;
}

TArray<FIntPoint> UTerrainLayoutFunctionLibrary::GetAdyacentCellsOfCell(FIntPoint CellIn)
{
	TArray<FIntPoint> AdyacentCells = TArray<FIntPoint>();

	FIntPoint CellLocal = { 0, 1 }; //up
	AdyacentCells.Add(CellLocal + CellIn);

	CellLocal = { 1, 0 }; //Right
	AdyacentCells.Add(CellLocal + CellIn);

	CellLocal = { 0, -1 };   //down
	AdyacentCells.Add(CellLocal + CellIn);

	CellLocal = { -1, 0 }; // Left
	AdyacentCells.Add(CellLocal + CellIn);

	return AdyacentCells;
}

FIntPoint UTerrainLayoutFunctionLibrary::GetAdyacentCellInDirectionOfCell(const ETerrainGen_CorridorDirection& DirectionIn, const FIntPoint& CellIn)
{
	TArray<FIntPoint> AdyacentCells = GetAdyacentCellsOfCell(CellIn);
	FIntPoint Cell = FIntPoint();

	switch (DirectionIn)
	{
	case ETerrainGen_CorridorDirection::Up: Cell = AdyacentCells[0];
		break;
	case ETerrainGen_CorridorDirection::Down: Cell = AdyacentCells[2];
		break;
	case ETerrainGen_CorridorDirection::Left: Cell = AdyacentCells[3];
		break;
	case ETerrainGen_CorridorDirection::Right: Cell = AdyacentCells[1];
		break;
	default:
		break;
	}

	return Cell;
}

FIntPoint UTerrainLayoutFunctionLibrary::GetCellIDInWorldLocation(FVector WorldLocationIn, float CellSizeIn, FVector InitialLocationIn)
{
	//Remove the initial location
	FVector Location = WorldLocationIn - InitialLocationIn;
	Location = Location / CellSizeIn;

	FIntPoint CellID;
	CellID.X = FMath::TruncToInt(Location.Y); //Reverse coords
	CellID.Y = FMath::TruncToInt(Location.X);

	return CellID;
}

FVector UTerrainLayoutFunctionLibrary::GetCellWorldPosition(FIntPoint CellsIDIn, float CellSizeIn, FVector2D AnchorIn)
{
	FVector Position;

	//Axis must be reversed for this to work(See landscape placement logic)
	Position.X = CellsIDIn.Y * CellSizeIn + CellSizeIn * AnchorIn.Y; //Show the location based on anchor
	Position.Y = CellsIDIn.X * CellSizeIn + CellSizeIn * AnchorIn.X;	
	
	Position.Z = 0;

	return Position;
}

TArray<FIntPoint> UTerrainLayoutFunctionLibrary::GetCellsInArea(const TMap<FIntPoint, FCellLayout>& CellsDataIn, float CellSizeIn, FVector InitialLocationIn, FVector WorldLocationIn, int32 CellRadious)
{
	TArray<FIntPoint> cells = TArray<FIntPoint>();
	const FIntPoint centralCell = GetCellIDInWorldLocation(WorldLocationIn, CellSizeIn, InitialLocationIn);		
	if (!CellsDataIn.Contains(centralCell))
	{
		return cells;
	}

	cells.Add(centralCell);
	if (CellRadious <= 0)
	{		
		return cells;
	}

	const int32 x_min = centralCell.X - CellRadious;
	const int32 x_max = centralCell.X + CellRadious;
	const int32 y_min = centralCell.Y - CellRadious;
	const int32 y_max = centralCell.Y + CellRadious;

	for (int32 i = x_min; i <= x_max; i++)
	{
		for (int32 j = y_min; j <= y_max; j++)
		{	
			if (!CellsDataIn.Contains({ i,j }))
			{
				continue;
			}

			cells.Add({ i,j });
		}
	}

	return cells;
}

float UTerrainLayoutFunctionLibrary::GetWorldDistanceBetweenCells(FIntPoint CellA, FIntPoint CellB, float CellSizeIn)
{
	const FVector WPA = GetCellWorldPosition(CellA, CellSizeIn);
	const FVector WPB = GetCellWorldPosition(CellB, CellSizeIn);

	return FVector::Dist2D(WPA, WPB);
}

FIntPoint UTerrainLayoutFunctionLibrary::GetMaxGridSize(const TArray<FIntPoint>& Cells)
{
	FIntPoint MaxValues = FIntPoint();

	for (const FIntPoint& cell : Cells)
	{
		if (cell.X > MaxValues.X)
		{
			MaxValues.X = cell.X;
		}
		if (cell.Y > MaxValues.Y)
		{
			MaxValues.Y = cell.Y;
		}		
	}	

	return MaxValues;
}

FIntPoint UTerrainLayoutFunctionLibrary::GetMinGridSize(const TArray<FIntPoint>& Cells)
{
	FIntPoint MinValues = FIntPoint(9999999, 999999);

	for (const FIntPoint& cell : Cells)
	{
		if (cell.X < MinValues.X)
		{
			MinValues.X = cell.X;
		}
		if (cell.Y < MinValues.Y)
		{
			MinValues.Y = cell.Y;
		}
	}

	return MinValues;
}

float UTerrainLayoutFunctionLibrary::GetCellDistanceBetweenCells(FIntPoint CellA, FIntPoint CellB)
{
	if (CellA == CellB)
	{
		return -1;
	}

	return UKismetMathLibrary::Distance2D(CellA, CellB) - 1; //I need to remove 1 since i consider distance 0 to be 1.
}

float UTerrainLayoutFunctionLibrary::GetMinimunCellDistanceBetweenCells(const TArray<FIntPoint>& CellsA, const TArray<FIntPoint>& CellsB)
{
	float MinimunDistance = 1000000;

	for (const FIntPoint& cellA : CellsA)
	{
		for (const FIntPoint& cellB : CellsB)
		{
			int32 CurrentDistance = GetCellDistanceBetweenCells(cellA, cellB);

			if ((CurrentDistance < MinimunDistance))
			{
				MinimunDistance = CurrentDistance;
			}

			if (MinimunDistance <= 0)
			{
				return MinimunDistance;
			}
		}
	}

	return MinimunDistance;
}

float UTerrainLayoutFunctionLibrary::GetRoomVectorAngle(FIntPoint RoomCentralCell, FIntPoint InitialCell)
{
	const FVector2D RoomCell = RoomCentralCell - InitialCell;
	const FVector2D BaseVector = FVector2D(1, 0);
	const float Dot = RoomCell.Dot(BaseVector);
	return FMath::Acos(Dot);
}
