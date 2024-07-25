//Copyright 2020 Marchetti Alfredo. All Rights Reserved.
#pragma once
#include "Layout/LayoutObjects/BaseLayoutObject.h"
#include "UObject/ObjectPtr.h"
#include "UObject/UObjectGlobals.h"
#include "LayoutObjectSquare.generated.h"

UCLASS()
class TERRAINGENERATOR_API ULayoutObjectSquare : public UBaseLayoutObject
{
	GENERATED_BODY()

public:
	void GenerateLayout_Implementation(int32 SeedIn, FLayoutInputData& InLayoutInputData) override;

protected:

};
