//Copyright 2020 Marchetti Alfredo. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "TerrainLayoutRoomData.generated.h"

class UTerrainBaseLayoutData;

UCLASS()
class TERRAINGENERATOR_API UTerrainLayoutRoomData : public UPrimaryDataAsset
{
	GENERATED_BODY()
				
public:
	FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("TerrainLayoutRoomData", GetFName());
	}

	/* The tags that this Base will have.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AssetBundles = "Room"), meta = (Categories = "Room"), Category = "Room")
	FGameplayTagContainer RoomTags;	

	/* How the room is generated. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AssetBundles = "Room"), meta = (Categories = "Room"), Category = "Room")
	UTerrainBaseLayoutData* RoomLayout;

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};
