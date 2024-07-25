//Copyright 2020 Marchetti Alfredo. All Rights Reserved.
#include "Layout/Data/TerrainLayoutData.h"

#include "Misc/DataValidation.h"

#define LOCTEXT_NAMESPACE "TerrainLayoutData"

#if WITH_EDITOR
EDataValidationResult UTerrainLayoutData::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);

	if (!InitialRoomsLayout)
	{
		Result = EDataValidationResult::Invalid;
		Context.AddError(FText(LOCTEXT("InitialRoomsLayoutInvalid", "InitialRoomsLayout is invalid")));
	}

	int32 EntryIndex = 0;
	for (const UTerrainLayoutRoomData* roomlayot : RoomLayouts)
	{
		if (!roomlayot)
		{
			Result = EDataValidationResult::Invalid;
			Context.AddError(FText::Format(LOCTEXT("RoomLayoutInvalid", "Null entry at index {0} in RoomLayouts"), FText::AsNumber(EntryIndex)));
		}

		++EntryIndex;
	}

	return Result;
}
#endif

#undef LOCTEXT_NAMESPACE

