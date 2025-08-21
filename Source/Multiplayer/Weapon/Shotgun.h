// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HitScanWeapon.h"
#include "Shotgun.generated.h"

UCLASS()
class MULTIPLAYER_API AShotgun : public AHitScanWeapon
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AShotgun();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void Fire(const FVector& HitTarget) override;
private:
	UPROPERTY(EditAnywhere, Category = "Weapon Scatter")
	uint32 NumberOfPellets = 10;
};
