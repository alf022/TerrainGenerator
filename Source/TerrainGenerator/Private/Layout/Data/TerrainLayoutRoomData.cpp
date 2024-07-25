//Copyright 2020 Marchetti Alfredo. All Rights Reserved.
#include "Layout/Data/TerrainLayoutRoomData.h"

#include "Misc/DataValidation.h"

#define LOCTEXT_NAMESPACE "TerrainLayoutRoomData"

#if WITH_EDITOR
EDataValidationResult UTerrainLayoutRoomData::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);

	if (!RoomLayout)
	{
		Result = EDataValidationResult::Invalid;
		Context.AddError(FText(LOCTEXT("RoomLayoutInvalid", "RoomLayout is invalid")));		
	}

	return Result;
}
#endif

#undef LOCTEXT_NAMESPACE
