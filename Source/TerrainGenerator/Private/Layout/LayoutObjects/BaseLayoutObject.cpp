//Copyright 2020 Marchetti Alfredo. All Rights Reserved.
#include "Layout/LayoutObjects/BaseLayoutObject.h"
#include "Engine/NetDriver.h"

UWorld* UBaseLayoutObject::GetWorld() const
{
	if (HasAllFlags(RF_ClassDefaultObject))
	{
		return nullptr;
	}
	return GetOuter()->GetWorld();
}

int32 UBaseLayoutObject::GetFunctionCallspace(UFunction* Function, FFrame* Stack)
{
	if (HasAnyFlags(RF_ClassDefaultObject) || !IsSupportedForNetworking())
	{
		return FunctionCallspace::Local;
	}
	check(GetOuter() != nullptr);
	return GetOuter()->GetFunctionCallspace(Function, Stack);
}

bool UBaseLayoutObject::CallRemoteFunction(UFunction* Function, void* Parameters, FOutParmRec* OutParms, FFrame* Stack)
{
	check(!HasAnyFlags(RF_ClassDefaultObject));
	check(GetOuter() != nullptr);

	AActor* Owner = CastChecked<AActor>(GetOuter());

	bool bProcessed = false;

	FWorldContext* const Context = GEngine->GetWorldContextFromWorld(GetWorld());
	if (Context != nullptr)
	{
		for (FNamedNetDriver& Driver : Context->ActiveNetDrivers)
		{
			if (Driver.NetDriver != nullptr && Driver.NetDriver->ShouldReplicateFunction(Owner, Function))
			{
				Driver.NetDriver->ProcessRemoteFunction(Owner, Function, Parameters, OutParms, Stack, this);
				bProcessed = true;
			}
		}
	}

	return bProcessed;
}

void UBaseLayoutObject::GenerateLayout_Implementation(int32 SeedIn, FLayoutInputData& InLayoutInputData)
{		
	InitializeStream(SeedIn);
	GenerateCellsAmount(InLayoutInputData);
}

void UBaseLayoutObject::InitializeStream(int32 SeedIn)
{	
	Seed = SeedIn;
	Stream = FRandomStream(Seed);
	Stream.Reset();
}

void UBaseLayoutObject::GenerateCellsAmount(const FLayoutInputData& InLayoutInputData)
{
	if (InLayoutInputData.Cells.Num() <= 0)
	{
		return;
	}

	ToGenerateCellsAmount = Stream.RandRange(InLayoutInputData.MinCells, InLayoutInputData.MaxCells);
	ToGenerateCellsAmount += InLayoutInputData.Cells.Num();

	if (bScaleGeneratedCellsBasedOnBounds)
	{
		ToGenerateCellsAmount *= InLayoutInputData.ToGenerateCellsScale * ScaleMultiplier;
	}
}

bool UBaseLayoutObject::IsCellInBounds(FIntPoint Cell, const FLayoutInputData& InLayoutInputData) const
{
	if (bIgnoreCellBounds)
	{
		return true;
	}

	return (InLayoutInputData.CellsBounds.Contains(Cell) || InLayoutInputData.CellsBounds.Num() <= 0);
}

TArray<FIntPoint> UBaseLayoutObject::GenerateCellsAtCellDistance(FIntPoint Cell, int32 Distance, const FLayoutInputData& InLayoutInputData, bool OnlyCorners, bool RandomizeOrder)
{
	int32 x_min = Cell.X - Distance;
	int32 x_min_clamp = FMath::Max(x_min, InLayoutInputData.Bounds_Min_X);

	int32 x_max = Cell.X + Distance;
	int32 x_max_clamp = FMath::Min(x_max, InLayoutInputData.Bounds_Max_X);

	int32 y_min = Cell.Y - Distance;
	int32 y_min_clamp = FMath::Max(y_min, InLayoutInputData.Bounds_Min_Y);

	int32 y_max = Cell.Y + Distance;
	int32 y_max_clamp = FMath::Min(y_max, InLayoutInputData.Bounds_Max_Y);

	TArray<FIntPoint> cells = TArray<FIntPoint>();
	for (int32 i = x_min_clamp; i <= x_max_clamp; i++)
	{
		for (int32 j = y_min_clamp; j <= y_max_clamp; j++)
		{
			bool bIsCorner = (i == x_min || i == x_max) && (j == y_min || j == y_max);

			if (OnlyCorners && bIsCorner)
			{
				cells.AddUnique(FIntPoint(i, j));
			}
			else if (!OnlyCorners && (i == x_min || i == x_max || j == y_min || j == y_max) && !bIsCorner)
			{
				cells.AddUnique(FIntPoint(i, j));
			}
		}
	}

	if (RandomizeOrder)
	{
		Algo::Sort(cells, [this](FIntPoint A, FIntPoint B)
			{
				return Stream.FRand() < .5f;
			});
	}

	return cells;
}
