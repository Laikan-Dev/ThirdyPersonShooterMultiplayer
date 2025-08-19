// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "MultiplayerGameMode.h"
#include "MPProjectile.h"
#include "BaseWeapon.h"
#include "InteractWithCrosshairs.h"
#include "PlayerOverlayStates.h"
#include "Components/TimelineComponent.h"
#include "Multiplayer/Enums/TurningInPlace.h"
#include "Multiplayer/Player/CombatState.h"
#include "MultiplayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class USkeletalMeshComponent;
class UInputMappingContext;
class UInputAction;
class UWidgetComponent;
class UNiagaraComponent;
class SceneCaptureComponent2D;
struct FInputActionValue;
struct FWeaponInformation;

UENUM(BlueprintType)
enum class ECharMovDirection : uint8
{
	None UMETA(DisplayName = "None"),
	Forward UMETA(DisplayName = "Forward"),
	Backward UMETA(DisplayName = "Backward"),
	Left UMETA(DisplayName = "Left"),
	Right UMETA(DisplayName = "Right"),
};

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class AMultiplayerCharacter : public ACharacter, public IInteractWithCrosshairs
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;
	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	//RenderTarget To UI
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USceneCaptureComponent2D* UICameraVisualize;
	//WeaponSocket
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* WeaponSocket;
	//WeaponSocket
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UNiagaraComponent* AbilityVFX;
	//WidgetComp
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))	
	UWidgetComponent* PlayerInfo;
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;
	/** Dash Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* DashAction;
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
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ReloadAction;
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
	virtual void PostInitializeComponents() override;
	UPROPERTY()
	class UMultiplayerCharAnimInstance* AnimInstance;

	void PlayFireMontage(bool bAiming);
	void PlayReloadMontage();
	virtual void OnRep_ReplicatedMovement() override;

	void Elim();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastElim();

	UPROPERTY(Replicated)
	bool bDisableGameplay = false;

protected:
	//Commands
	/** Called for movement input */
	void Move(const FInputActionValue& Value);
	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	//Reload
	void ReloadButtonPressed();

	//HitReact
	void PlayHitReactMontage();
	void PlayDeathMontage();

	//Damage
	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, class AController* InstigatedController, class AActor* DamageCauser);
	void UpdateHUDHealth();
	//Aiming
	UFUNCTION()
	void StartAiming();
	UFUNCTION()
	void StopAiming();

	//Firing
	void FireButtonPressed();
	void FireButtonReleased();

	//Proxies
	void SimProxiesTurn();
	void CalculateAO_Pitch();
	//Jump
	UFUNCTION()
	void StartJump();

	//Poll for ani relevant class spawning
	void PollInit();
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

	//DashProperties
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "DashDefaults")
	float TimeDashCooldown;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "DashDefaults")
	int32 DashForceXY;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "DashDefaults")
	float DashForceZ;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "DashDefaults")
	bool bIsDashing;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "DashDefaults")
	ECharMovDirection LastDirection;

	//DashFunctions
	UFUNCTION()
	void StartDash();
	UFUNCTION(BlueprintCallable)
	void AddDashImpulse(ECharMovDirection Direction);
	
	UFUNCTION()
	void DashCooldown();
	UFUNCTION()
	bool CanDash();
	UFUNCTION()
	void AbilityDash(ECharMovDirection Direction);
	UFUNCTION(Server, Unreliable)
	void Server_AbilityDash(ECharMovDirection Direction);
	UFUNCTION(NetMulticast, Reliable)
	void MC_AbilityDash(ECharMovDirection Direction);

	//Pickup
	UFUNCTION()
	void EquipItem();

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

	virtual void Destroyed() override;

	//To Check Camera
	virtual void Tick(float DeltaTime) override;

	//Camera Update
	void UpdateCamera();

private:
	void RotateInPlace(float DeltaTime);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCombatComponent* CombatSystem;
	UFUNCTION(Server, Reliable)
	void ServerEquipItem();
	void HideCameraIfCharacterClose();
	UPROPERTY(EditAnywhere)
	float CameraThreshold = 200.f;
	
	bool bRotateRootBone;
	float TurnThreshold = 0.5f;
	FRotator ProxyRotationLastFrame;
	FRotator ProxyRotation;
	float ProxyRotationYaw;
	float TimeSinceLastMovementReplication;
	float CalculateSpeed();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	//** Return bRotateRootBone**/
	FORCEINLINE bool ShouldRotateRootBone() const { return bRotateRootBone; }

	FORCEINLINE UCombatComponent* GetCombatSystem() const { return CombatSystem; }

	FORCEINLINE bool GetDisableGameplay() const { return bDisableGameplay; }

	UPROPERTY(ReplicatedUsing = OnRep_PlayerTeam, BlueprintReadWrite)
	ETeam CurrentTeam = ETeam::ET_NoTeam;

	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	class ABaseWeapon* OverlappingWeapon;

	void SetOverlappingWeapon(ABaseWeapon* Weapon);

	UFUNCTION()
	void OnRep_OverlappingWeapon(ABaseWeapon* LastWeapon);

	bool IWeaponEquipped();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	EPlayerOverlayState CurrentState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_CurrentWeapon)
	class UWeaponsDataAsset* WeaponData;

protected:
//ReplicatedProperties
	//Health
	UPROPERTY(EditDefaultsOnly, Category = "PlayerStats")
	float MaxHealth = 100.f;
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_CurrentHealth, Category = PlayerStats)
	float CurrentHealth;

	bool bIsDead = false;

	UPROPERTY()
	class AMultiplayerPlayerController* MultiplayerPlayerController;

	//Death
	bool bElimmed = false;
	FTimerHandle ElimTimer;
	void ElimTimerFinished();
	UPROPERTY(EditDefaultsOnly)
	float ElimDelay = 3.f;

	//DissolveEffect
	UPROPERTY(VisibleAnywhere)
	UTimelineComponent* DissolveTimeline;
	FOnTimelineFloat DissolveTrack;
	UPROPERTY(EditDefaultsOnly)
	UCurveFloat* DissolveCurve;
	//Dynamic instance that we can change at runtime
	UPROPERTY(VisibleAnywhere)
	UMaterialInstanceDynamic* DynamicDissolveMaterialInstance1;
	UPROPERTY(VisibleAnywhere)
	UMaterialInstanceDynamic* DynamicDissolveMaterialInstance2;

	class AChaosRemPlayerState* PossessedPlayerState;
	
	// Material instance set on blueprints, used with the dynamic material instance
	UPROPERTY(EditAnywhere)
	UMaterialInstance* DissolveMaterialInstance1;
	UPROPERTY(EditAnywhere)
	UMaterialInstance* DissolveMaterialInstance2;
	UFUNCTION()
	void UpdateDissolveMaterial(float DissolveValue);
	void StartDissolve();

	//Movement
	UPROPERTY()
	bool bIsCrounch;
	//Rep Functions for Aiming
	UFUNCTION()
	void OnRep_Aiming();

	UFUNCTION(Server, Reliable)
	void ServerSetRuning(bool bIsRunning);
	void ServerSetRuning_Implementation(bool bIsRunning);
//FunctionRep for Health
	UFUNCTION()
	void OnRep_CurrentHealth();

	//FunctionRep
	UFUNCTION()
	void OnRep_PlayerTeam();
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//FunctionRep for Weapon
	UFUNCTION()
	void OnRep_CurrentWeapon();
	void OnCurrentWeaponUpdate();

public:
	//Getter for max health
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }

	//Getter for current health
	UFUNCTION(BlueprintPure, Category = "Health")
	FORCEINLINE float GetCurrentHealth() const { return CurrentHealth; }

	UFUNCTION(BlueprintCallable, Category = "Health")
	void SetCurrentHealth(float healthValue);

	UFUNCTION()
	void SetCurrentWeapon(UWeaponsDataAsset* CurrentWeapon);

	UFUNCTION(Server, Reliable)
	void Server_SetCurrentWeapon(UWeaponsDataAsset* CurrentWeapon);

	UFUNCTION(NetMulticast, Reliable)
	void MC_SetCurrentWeapon(UWeaponsDataAsset* CurrentWeapon);

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
	void Server_HandleFire(UAnimMontage* FireAnim, FVector MuzzleVector, FRotator MuzzleRotation);

	UFUNCTION(Server, Reliable)
	void ServerSetTeam(ETeam NewTeam);
	void ServerSetTeam_Implementation(ETeam NewTeam);
	
	//bool ServerSetTeam_Validation(ETeam NewTeam);

	UFUNCTION(Client, Reliable)
	void SelectTeam();

	FTimerHandle FiringTimer;

	void AimOffset(float DeltaTime);
	
	float AO_Yaw;
	float InterpAO_Yaw;
	float AO_Pitch;
	FRotator StartAimRotation;


	ETurningInPlace TurningInPlace;

	UFUNCTION()
	void TurnInPlace(float DeltaTime);

public:

	bool bIsAiming();
	//TeamColor
	UPROPERTY(EditDefaultsOnly)
	UMaterial* Red;
	UPROPERTY(EditDefaultsOnly)
	UMaterial* Blue;

	//Widgets
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> SelectTeamWidget;

	//Movement Properties
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	float NormalVelocity = 500.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	float AimingVelocity = 250.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	float RunningVelocity = 700.f;

	//WeaponInfo
	TObjectPtr<UAnimationAsset> WeaponShotAnim;
	
	//Montages
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Montages")
	UAnimMontage* DashForward;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Montages")
	UAnimMontage* DashBackward;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Montages")
	UAnimMontage* DashLeft;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Montages")
	UAnimMontage* DashRight;
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* HitReactMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* DeathMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* ShootingMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* ReloadMontage;
	
	//VFX
	UFUNCTION()
	void OnAbilityVFX(bool bNewActivate);

	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }
	ABaseWeapon* GetEquippedWeapon();

	FORCEINLINE ETurningInPlace GetTurningInPlace() const { return TurningInPlace; }
	FORCEINLINE bool IsElimmed() const {return bElimmed; }
	FVector GetHitTarget() const;

	ECombatState GetCombatState() const;
};


