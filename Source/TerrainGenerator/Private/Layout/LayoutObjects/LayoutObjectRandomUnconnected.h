//Copyright 2020 Marchetti Alfredo. All Rights Reserved.
#pragma once
#include "Layout/LayoutObjects/BaseLayoutObject.h"
#include "UObject/ObjectPtr.h"
#include "UObject/UObjectGlobals.h"
#include "LayoutObjectRandomUnconnected.generated.h"

UCLASS()
class TERRAINGENERATOR_API ULayoutObjectRandomUnconnected : public UBaseLayoutObject
{
	GENERATED_BODY()

public:
	void GenerateLayout_Implementation(int32 SeedIn, FLayoutInputData& InLayoutInputData) override;

protected:
	/* If should ignore the quad bounds and only use the cell bounds.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Layout")
	bool bIgnoreQuadBounds = false;
};
