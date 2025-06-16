// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "MultiplayerHud.h"
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

	//Replicates
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//Setup Player and Weapon
	friend class AMultiplayerCharacter;
	void EquipWeapon(class ABaseWeapon* WeaponToEquip);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	//Actions
	void SetAiming(bool bIsAiming);
	UFUNCTION(Server, Reliable)
	void Server_SetAiming(bool bIsAiming);

	void FireButtonPressed(bool bPressed);

private:
	//References
	class AMultiplayerCharacter* Character;
	class AMultiplayerPlayerController* PlayerController;
	class AMultiplayerHud* HUD;

	//PlayerStats
	UPROPERTY(Replicated)
	ABaseWeapon* EquippedWeapon;
	UPROPERTY(Replicated)
	bool bAiming;
	UPROPERTY(EditDefaultsOnly)
	float BaseWalkSpeed;
	UPROPERTY(EditDefaultsOnly)
	float AimWalkSpeed;
	bool bFireButtonPressed;

	//Crosshair Config
	void TraceUnderCrosshairs(FHitResult& TraceHitResult);
	float CrosshairVelocityFactor;
	float CrosshairInAirFactor;
	float CrosshairAimFactor;
	float CrosshairShootingFactor;
	void SetHUDCrosshairs(float DeltaTime);

	FHUDPackage HUDPackage;

	//Fire
	UFUNCTION(Server, Reliable)
	void ServerFire(const FVector_NetQuantize& TraceHitTarget);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire(const FVector_NetQuantize& TraceHitTarget);

	FVector HitTarget;

	float DefaultFOV;
	UPROPERTY(EditAnywhere, Category = "Combat")
	float ZoomedFOV = 30.f;

	float CurrentFOV;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float ZoomInterpSpeed;

	void InterpFOV(float DeltaTime);
};
