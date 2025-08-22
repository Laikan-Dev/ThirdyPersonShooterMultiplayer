// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MPProjectile.h"
#include "ProjectileRocket.generated.h"

UCLASS()
class MULTIPLAYER_API AProjectileRocket : public AMPProjectile
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AProjectileRocket();
	virtual void Destroyed() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void OnProjectileImpact(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;


	UPROPERTY(VisibleAnywhere)
	class URocketMovementComponent* RocketMovementComponent;
	

	UPROPERTY(EditAnywhere)
	USoundCue* ProjectileLoop;

	UPROPERTY()
	UAudioComponent* ProjectileLoopComp;

	UPROPERTY(EditAnywhere)
	USoundAttenuation* LoopingSoundAttenuation;
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	

private:

};
