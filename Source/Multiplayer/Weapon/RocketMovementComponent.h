// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "GameFramework/ProjectileMovementComponent.h"
#include "RocketMovementComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MULTIPLAYER_API URocketMovementComponent : public UProjectileMovementComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	URocketMovementComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual EHandleBlockingHitResult HandleBlockingHit(const FHitResult& Hit, float TimeTick, const FVector& MoveDelta, float& SubTickTimeRemaining) override;
	virtual void HandleImpact(const FHitResult& Hit, float TimeSlice = 0, const FVector& MoveDelta = FVector::ZeroVector) override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
