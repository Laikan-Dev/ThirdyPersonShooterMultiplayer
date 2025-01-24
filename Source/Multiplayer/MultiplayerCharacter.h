// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "MultiplayerGameMode.h"
#include "MultiplayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;


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
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* BlueInput;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* RedInput;

public:
	AMultiplayerCharacter();
	

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
	UFUNCTION(BlueprintCallable)
	void ChoseRed();
	UFUNCTION(BlueprintCallable)
	void ChoseBlue();

	//Inputs
	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputAction* FireInput;
	
	

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UPROPERTY(ReplicatedUsing = OnRep_PlayerTeam, BlueprintReadWrite)
	ETeam CurrentTeam = ETeam::ET_NoTeam;
protected:

	UPROPERTY(EditDefaultsOnly, Category = "Health")
	float MaxHealth;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentHealth)
	float CurrentHealth;

	UPROPERTY(ReplicatedUsing = OnRep_OnDeath)
	bool bIsDead;

	UFUNCTION()
	void OnRep_CurrentHealth();
	UFUNCTION()
	void OnRep_OnDeath();
	void OnDeathUpdate();
	UFUNCTION()
	void OnRep_PlayerTeam();
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void OnHealthUpdate();



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
	void ActiveRagdol();

	UFUNCTION(Server, Reliable)
	void HandleFire();

	UFUNCTION(Server, Reliable)
	void ServerSetTeam(ETeam NewTeam);
	void ServerSetTeam_Implementation(ETeam NewTeam);
	
	//bool ServerSetTeam_Validation(ETeam NewTeam);

	UFUNCTION(Client, Unreliable)
	void SelectTeam();

	FTimerHandle FiringTimer;

public:
	UPROPERTY(EditDefaultsOnly)
	UMaterial* Red;
	UPROPERTY(EditDefaultsOnly)
	UMaterial* Blue;

	//Widgets
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> SelectTeamWidget;
};


