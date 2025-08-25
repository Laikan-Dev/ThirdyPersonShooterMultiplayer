// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Pickup.h"
#include "Multiplayer/Weapon/WeaponTypes.h"
#include "AmmoPickup.generated.h"

UCLASS()
class MULTIPLAYER_API AAmmoPickup : public APickup
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAmmoPickup();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

private:
	UPROPERTY(EditAnywhere)
	int32 AmmoAmount = 30;

	UPROPERTY(EditAnywhere)
	EWeaponType WeaponType;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
