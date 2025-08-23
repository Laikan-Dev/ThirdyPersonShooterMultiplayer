// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MultiplayerHud.h"
#include "Components/ActorComponent.h"
#include "Multiplayer/Weapon/WeaponTypes.h"
#include  "Multiplayer/Player/CombatState.h"
#include "CombatComponent.generated.h"


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
	void Reload();
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void FinishReload();
	void UpdateAmmoValues();
	void FireButtonPressed(bool bPressed);

	void ThrowGrenade();
	void ShowAttachedGrenade(bool bShowGrenade);
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void LaunchGrenade();

	UFUNCTION(Server, Reliable)
	void ServerThrowGrenade();
	UFUNCTION(BlueprintCallable)
	void ThrowGrenadeFinished();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	void SetAiming(bool bIsAiming);
	UFUNCTION(Server, Reliable)
	void Server_SetAiming(bool bIsAiming);
	UFUNCTION()
	void OnRep_EquippedWeapon();
	UFUNCTION(Server, Reliable)
	void ServerReload();

	int32 AmountToReload();

	void HandleReload();

	
	void Fire();

	FTimerHandle FireTimer;
	bool bCanFire = true;
	bool CanFire();

	// Carried ammo for the currently-equipped weapon
	UPROPERTY(ReplicatedUsing = OnRep_CarriedAmmo)
	int32 CarriedAmmo;
	
	UFUNCTION()
	void OnRep_CarriedAmmo();

	TMap<EWeaponType, int32> CarriedAmmoMap;

	UPROPERTY(EditAnywhere)
	int32 StartARAmmo = 30;
	UPROPERTY(EditAnywhere)
	int32 StartRocketAmmo = 0;
	UPROPERTY(EditAnywhere)
	int32 StartPistolAmmo = 0;
	UPROPERTY(EditAnywhere)
	int32 StartSMGAmmo = 0;
	UPROPERTY(EditAnywhere)
	int32 StartShotgunAmmo = 0;
	UPROPERTY(EditAnywhere)
	int32 StartSniperRifleAmmo = 0;
	UPROPERTY(EditAnywhere)
	int32 StartGrenadeLauncherAmmo = 0;
	void InitializeCarriedAmmo();
	UPROPERTY(ReplicatedUsing = OnRep_CombatState)
	ECombatState CombatState = ECombatState::ECS_Idle;
	UFUNCTION()
	void OnRep_CombatState();

	void StartFireTimer();
	void FireTimerFinished();

private:
	UPROPERTY()
	class AMultiplayerCharacter* Character;
	UPROPERTY()
	class AMultiplayerPlayerController* PlayerController;
	UPROPERTY()
	class AMultiplayerHud* HUD;
	
	FHUDPackage HUDPackage;
	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
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
