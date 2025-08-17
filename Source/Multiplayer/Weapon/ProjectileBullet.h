// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MPProjectile.h"
#include "ProjectileBullet.generated.h"

UCLASS()
class MULTIPLAYER_API AProjectileBullet : public AMPProjectile
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AProjectileBullet();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
virtual void OnProjectileImpact(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
