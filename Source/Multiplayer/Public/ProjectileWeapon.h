// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseWeapon.h"
#include "ProjectileWeapon.generated.h"

/**
 * 
 */
class AMPProjectile;
UCLASS()
class MULTIPLAYER_API AProjectileWeapon : public ABaseWeapon
{
	GENERATED_BODY()
	
public:
	virtual void Fire(const FVector& HitTarget) override;

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<AMPProjectile> ProjectileClass;
};
