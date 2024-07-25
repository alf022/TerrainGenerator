//Copyright 2020 Marchetti Alfredo. All Rights Reserved.
#pragma once
#include "Layout/Types/LayoutTypes.h"
#include "UObject/Object.h"
#include "UObject/ObjectPtr.h"
#include "UObject/UObjectGlobals.h"

#include "BaseLayoutObject.generated.h"

// Represents a data extension for game items.
UCLASS(Abstract, Blueprintable, EditInlineNew, Meta = (DisplayName = "Base Layout Object"))
class TERRAINGENERATOR_API UBaseLayoutObject : public UObject
{
	GENERATED_BODY()

public:
	virtual class UWorld* GetWorld() const override;
	virtual int32 GetFunctionCallspace(UFunction* Function, FFrame* Stack) override;
	virtual bool CallRemoteFunction(UFunction* Function, void* Parameters, FOutParmRec* OutParms, FFrame* Stack) override;

	/**
	*	Overrideable function for layout generation.
	*/
	UFUNCTION(BlueprintNativeEvent, Category = "Layout")
	void GenerateLayout(int32 SeedIn, FLayoutInputData& InLayoutInputData);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Layout")
	bool bIgnoreCellBounds = false;

	/* If the amount of cells generated should change when provided with a bounds array.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Layout")
	bool bScaleGeneratedCellsBasedOnBounds = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Layout")
	float ScaleMultiplier = 1.f;

	UPROPERTY()
	int32 ToGenerateCellsAmount = 0;

	UPROPERTY()
	int32 Seed;

	UPROPERTY()
	FRandomStream Stream;

	void InitializeStream(int32 SeedIn);
	void GenerateCellsAmount(const FLayoutInputData& InLayoutInputData);

	bool IsCellInBounds(FIntPoint Cell, const FLayoutInputData& InLayoutInputData) const;
	TArray <FIntPoint> GenerateCellsAtCellDistance(FIntPoint Cell, int32 Distance, const FLayoutInputData& InLayoutInputData, bool OnlyCorners  = false, bool RandomizeOrder = true);
};
