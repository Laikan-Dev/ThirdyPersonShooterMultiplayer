#pragma once

USTRUCT(BlueprintType)
struct WeaponInformation
{
	GENERATED_BODY()

	UPROPERTY()
	TSubclassOf<ABaseWeapon> WeapontClass;
	UPROPERTY()
	FName Name;


};
