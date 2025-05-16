// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BaseWeapon.h"
#include "WeaponsDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYER_API UWeaponsDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FWeaponInformation WeaponStats;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FCrosshairInfo CrosshairInfo;

public:
	FWeaponInformation GetWeaponStats();
	FCrosshairInfo GetCrosshairInfo();
};
