//Copyright 2020 Marchetti Alfredo. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "BaseTerrainEditor.generated.h"

UCLASS(Config = Game)
class TERRAINGENERATORED_API UBaseTerrainEditor : public UEditorUtilityWidget
{
	GENERATED_BODY()  
		
public:
	virtual const FText GetPaletteCategory() override;

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
};
