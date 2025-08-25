// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Pickup.h"
#include "SpeedPickup.generated.h"

UCLASS()
class MULTIPLAYER_API ASpeedPickup : public APickup
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASpeedPickup();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
private:
	UPROPERTY(EditAnywhere)
	float BaseSpeedBuff = 1600.f;
	UPROPERTY(EditAnywhere)
	float ChrouchSpeedBuff = 850.f;
	UPROPERTY(EditAnywhere)
	float SpeedBuffTime = 30.f;
};
