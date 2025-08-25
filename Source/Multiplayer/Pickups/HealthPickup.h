// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Pickup.h"
#include "HealthPickup.generated.h"

UCLASS()
class MULTIPLAYER_API AHealthPickup : public APickup
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AHealthPickup();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere)
	float HealAmount = 100.f;
	
	UPROPERTY(EditAnywhere)
	float HealingTime = 5.f;
	
};
