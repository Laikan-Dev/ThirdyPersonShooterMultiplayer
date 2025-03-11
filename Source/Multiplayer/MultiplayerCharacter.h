// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "MultiplayerGameMode.h"
#include "MPProjectile.h"
#include "BaseWeapon.h"
#include "PlayerOverlayStates.h"
#include "MultiplayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class USkeletalMeshComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
struct FWeaponInformation;


DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class AMultiplayerCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;
	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	//WeaponSocket
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* WeaponSocket;
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;
	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;
	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;
	//** Running Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* RunningAction;
	//** Crounch Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* CrounchAction;
	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;
	//FireInputs
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* FireInput;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* AimingInput;
	//PickUpInput
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* PickUpInput;

public:
	AMultiplayerCharacter();

protected:
	//Commands
	/** Called for movement input */
	void Move(const FInputActionValue& Value);
	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	//Aiming
	UFUNCTION()
	void StartAiming();
	UFUNCTION()
	void StopAiming();

	//Jump
	UFUNCTION()
	void StartJump();

	//Select Team
	UFUNCTION(BlueprintCallable)
	void ChoseRed();
	UFUNCTION(BlueprintCallable)
	void ChoseBlue();

	//Movement
	UFUNCTION()
	void Running();
	UFUNCTION()
	void StopRunning();
	UFUNCTION()
	void StartCrounch();
	UFUNCTION()
	void StopCrounch();

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

	//To Check Camera
	virtual void Tick(float DeltaTime) override;

	//Camera Update
	void UpdateCamera();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UPROPERTY(ReplicatedUsing = OnRep_PlayerTeam, BlueprintReadWrite)
	ETeam CurrentTeam = ETeam::ET_NoTeam;
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_CurrentWeapon)
	TSubclassOf<class ABaseWeapon> CurrentWeaponClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	EPlayerOverlayState CurrentState;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_CurrentWeapon)
	FWeaponInformation WeaponInfo;

protected:
//ReplicatedProperties
	//Health
	UPROPERTY(EditDefaultsOnly, Category = "Health")
	float MaxHealth;
	UPROPERTY(ReplicatedUsing = OnRep_CurrentHealth)
	float CurrentHealth;

	//Death
	UPROPERTY(Replicated)
	bool bIsDead;

	//Movement
	UPROPERTY()
	bool bIsCrounch;
	UPROPERTY(ReplicatedUsing = OnRep_Aiming, BlueprintReadOnly)
	bool bIsAiming;
	//Rep Functions for Aiming
	UFUNCTION()
	void OnRep_Aiming();
	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool bNewAiming);
	void ServerSetAiming_Implementation(bool bNewAiming);

	UFUNCTION(Server, Reliable)
	void ServerSetRuning(bool bIsRunning);
	void ServerSetRuning_Implementation(bool bIsRunning);
	UFUNCTION(Server, Reliable)
	void ServerSetCrounch(bool bIsCrouching);
	void ServerSetCrounch_Implementation(bool bIsRunning);

//FunctionRep for Health
	UFUNCTION()
	void OnRep_CurrentHealth();
	void OnDeathUpdate();

	//FunctionRep for Death
	UFUNCTION(Server, NetMulticast, Reliable)
	void MulticastOnDeath();

	//FunctionRep for Team
	UFUNCTION()
	void OnRep_PlayerTeam();
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void OnHealthUpdate();

	//FunctionRep for Weapon
	UFUNCTION()
	void OnRep_CurrentWeapon();
	void OnCurrentWeaponUpdate();

public:
	//Getter for max health
	UFUNCTION(BlueprintPure, Category = "Health")
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }

	//Getter for current health
	UFUNCTION(BlueprintPure, Category = "Health")
	FORCEINLINE float GetCurrentHealth() const { return CurrentHealth; }

	UFUNCTION(BlueprintCallable, Category = "Health")
	void SetCurrentHealth(float healthValue);

	UFUNCTION(BlueprintCallable, Category = "Health")
	float TakeDamage(float DamageTaken, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(Server, NetMulticast, Reliable)
	void SetCurrentWeapon(FWeaponInformation CurrentWeapon);

protected:
	UPROPERTY(EditDefaultsOnly, Category="Gameplay|Projectile")
	TSubclassOf<class AMPProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category="Gameplay")
	float FireRate;

	bool bIsFiringWeapon;

	UFUNCTION(BlueprintCallable, Category="Gameplay")
	void StartFire();

	UFUNCTION(BlueprintCallable, Category="Gameplay")
	void StopFire();

	UFUNCTION(Server, Reliable)
	void HandleFire(UAnimationAsset* FireAnim, FVector MuzzleVector, FRotator MuzzleRotation);

	UFUNCTION(Server, Reliable)
	void ServerSetTeam(ETeam NewTeam);
	void ServerSetTeam_Implementation(ETeam NewTeam);
	
	//bool ServerSetTeam_Validation(ETeam NewTeam);

	UFUNCTION(Client, Reliable)
	void SelectTeam();

	FTimerHandle FiringTimer;

public:
	//TeamColor
	UPROPERTY(EditDefaultsOnly)
	UMaterial* Red;
	UPROPERTY(EditDefaultsOnly)
	UMaterial* Blue;

	//Widgets
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> SelectTeamWidget;

	//Animation
	UPROPERTY(EditDefaultsOnly, Replicated, Category = "Montages")
	UAnimMontage* AimingMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* ShootingMontage;

	//Movement Properties
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	float NormalVelocity = 500.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	float AimingVelocity = 250.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	float RunningVelocity = 700.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	float CrounchVelocity = 200.f;

	//WeaponInfo
	UAnimationAsset* WeaponShotAnim;
	

};


