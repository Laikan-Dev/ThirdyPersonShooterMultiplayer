// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MPProjectile.h"
#include "ProjectileGrenade.generated.h"

UCLASS()
class MULTIPLAYER_API AProjectileGrenade : public AMPProjectile
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AProjectileGrenade();
	virtual void Destroyed() override;
	virtual void OnProjectileImpact(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UFUNCTION()
	void OnBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere)
	USoundCue* BounceSound;
};
