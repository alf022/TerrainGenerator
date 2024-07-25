//Copyright 2020 Marchetti Alfredo. All Rights Reserved.
#pragma once
#include "Layout/LayoutObjects/BaseLayoutObject.h"
#include "UObject/ObjectPtr.h"
#include "UObject/UObjectGlobals.h"
#include "LayoutObjectBorder.generated.h"

UCLASS()
class TERRAINGENERATOR_API ULayoutObjectBorder : public UBaseLayoutObject
{
	GENERATED_BODY()

public:
	void GenerateLayout_Implementation(int32 SeedIn, FLayoutInputData& InLayoutInputData) override;
	
protected:
	/* The size of the border.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layout")
	int32 Range = 1;
};
