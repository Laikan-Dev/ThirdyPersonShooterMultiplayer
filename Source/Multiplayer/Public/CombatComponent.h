// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MultiplayerHud.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

#define TRACE_LENGHT 80000.f;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MULTIPLAYER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCombatComponent();
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	friend class AMultiplayerCharacter;

	void EquipWeapon(class ABaseWeapon* WeaponToEquip);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	void SetAiming(bool bIsAiming);
	UFUNCTION(Server, Reliable)
	void Server_SetAiming(bool bIsAiming);

	

	void FireButtonPressed(bool bPressed);
	void Fire();

	FTimerHandle FireTimer;
	bool bCanFire = true;

	void StartFireTimer();
	void FireTimerFinished();

private:
	class AMultiplayerCharacter* Character;
	class AMultiplayerPlayerController* PlayerController;
	class AMultiplayerHud* HUD;
	FHUDPackage HUDPackage;
	UPROPERTY(Replicated)
	ABaseWeapon* EquippedWeapon;
	UPROPERTY(Replicated)
	bool bAiming;
	UPROPERTY(EditDefaultsOnly)
	float BaseWalkSpeed;
	UPROPERTY(EditDefaultsOnly)
	float AimWalkSpeed;

	bool bFireButtonPressed;

	float CrosshairVelocityFactor;
	float CrosshairInAirFactor;
	float CrosshairAimFactor;
	float CrosshairShootingFactor;

	UFUNCTION(Server, Reliable)
	void ServerFire(const FVector_NetQuantize& TraceHitTarget);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire(const FVector_NetQuantize& TraceHitTarget);

	void TraceUnderCrosshairs(FHitResult& TraceHitResult);

	void SetHUDCrosshairs(float DeltaTime);

	FVector HitTarget;

	float DefaultFOV;
	UPROPERTY(EditDefaultsOnly)

	
	float CurrentFOV;

	float ZoomedFOV = 30.f;
	UPROPERTY(EditDefaultsOnly)
	float ZoomInterpSpeed = 20.f;

	void InterpFOV(float DeltaTime);
public:	
	

		
};
