//Copyright 2020 Marchetti Alfredo. All Rights Reserved.
#include "Terrain/Data/TerrainData.h"

#include "Misc/DataValidation.h"

#define LOCTEXT_NAMESPACE "TerrainData"

#if WITH_EDITOR
EDataValidationResult UTerrainData::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);

	if (!TerrainLayoutData)
	{
		Result = EDataValidationResult::Invalid;
		Context.AddError(FText(LOCTEXT("TerrainLayoutDataInvalid", "TerrainLayoutData is invalid. Terrain layout will not be generated.")));
	}

	return Result;
}
#endif

#undef LOCTEXT_NAMESPACE

