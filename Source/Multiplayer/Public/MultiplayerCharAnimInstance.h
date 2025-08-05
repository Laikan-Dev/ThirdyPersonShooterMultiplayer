// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Multiplayer/Enums/TurningInPlace.h"
#include "MultiplayerCharAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYER_API UMultiplayerCharAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTimeX) override;

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class AMultiplayerCharacter* Character;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UCharacterMovementComponent* CharMovComponent;

public:
	//Properties
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MovementData")
	float GroundSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MovementData")
	FVector Velocity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MovementData")
	bool bShouldMove;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MovementData")
	bool bIsFalling;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bWeaponEquipped;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bIsCrouched;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bIsAiming;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AimOffset")
	float AimOffset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AimOffset")
	float YawOffset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AimOffset")
	float Lean;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AimOffset")
	float AO_Yaw;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AimOffset")
	float AO_Pitch;

	FRotator CharacterRotationLastFrame;
	FRotator CharacterRotation;
	FRotator DeltaRotation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MovementData")
	FTransform LeftHandTransform;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MovementData")
	FRotator RightHandRot;

	class ABaseWeapon* EquippedWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MovementData")
	float Direction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MovementData")
	ECharMovDirection MoveDirection;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MovementData")
	ECharMovDirection MoveDirection_LastUpdate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MovementData")

	ETurningInPlace TurningInPlace;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MovementData")
	bool bIsLocallyControlled;

	//Functions
	UFUNCTION(BlueprintCallable)
	void GetAimOffset();

	UFUNCTION(BlueprintCallable)
	void GetDirection();

	UFUNCTION(BlueprintCallable)
	void SetMovDirection(float DirectionValue, float SpeedValue);

	UFUNCTION(BlueprintCallable)
	void PlayDashMontage(ECharMovDirection AnimDirection);

};
