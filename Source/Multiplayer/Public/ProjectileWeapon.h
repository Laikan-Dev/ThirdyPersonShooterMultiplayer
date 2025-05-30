// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BaseWeapon.h"
#include "ProjectileWeapon.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYER_API AProjectileWeapon : public ABaseWeapon
{
	GENERATED_BODY()
	
public:
	virtual void Fire(const FVector& HitTarget) override;

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AMPProjectile> ProjectileClass;
};
