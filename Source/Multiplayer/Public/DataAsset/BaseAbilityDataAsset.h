// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BaseAbilityDataAsset.generated.h"

UENUM(BlueprintType)
enum class EAbilityType : uint8
{
	None,
	Movement,
	Healing,
	Shield,
	Damage
};
/**
 * 
 */
UCLASS()
class MULTIPLAYER_API UBaseAbilityDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AbilityInfo")
	FName AbilityName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AbilityInfo")
	FText AbilityDescryption;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AbilityInfo")
	UTexture2D* AbilityIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AbilityInfo")
	EAbilityType AbilityType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AbilityInfo")
	float AbilityCooldown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AbilityInfo")
	class UNiagaraSystem* AbilityVFX;
};
