// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerCharAnimInstance.h"
#include "Multiplayer/MultiplayerCharacter.h"
#include "Gameframework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UMultiplayerCharAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	Character = Cast<AMultiplayerCharacter>(GetOwningActor());
	if (Character)
	{
		CharMovComponent = Character->GetCharacterMovement();
	}
}

void UMultiplayerCharAnimInstance::NativeUpdateAnimation(float DeltaTimeX)
{
	Super::NativeUpdateAnimation(DeltaTimeX);
	if (Character && CharMovComponent)
	{
		//Set velocity and ground speed from the movement components velocity. Ground speed is calculated from only the X and Y axis of the velocity, so moving up or down does not affect it.
		Velocity = CharMovComponent->Velocity;
		GroundSpeed = UKismetMathLibrary::VSizeXY(Velocity);

		//Set Should Move to true only if ground speed is above a small threshold (to prevent incredibly small velocities from triggering animations) and if there is currently acceleration (input) applied.
		FVector Acceleration(0.0, 0.0, 0.0);
		bShouldMove = GroundSpeed > 3.0 && UKismetMathLibrary::NotEqual_VectorVector(CharMovComponent->GetCurrentAcceleration(), Acceleration, 0.0);

		//Set Is Falling from the movement components falling state.
		bIsFalling = CharMovComponent->IsFalling();
		bWeaponEquipped = Character->IWeaponEquipped();
		bIsCrouched = Character->bIsCrouched;
		bIsAiming = Character->bIsAiming();

		GetAimOffset();
		GetDirection();
		SetMovDirection(Direction, GroundSpeed);
	}
}

void UMultiplayerCharAnimInstance::GetAimOffset()
{
	if (Character->bIsAiming())
	{
		FRotator AimRotator = UKismetMathLibrary::NormalizedDeltaRotator(Character->GetBaseAimRotation(), Character->GetActorRotation());
		float Value = AimRotator.Pitch * -1.0;
		AimOffset = FMath::Clamp(Value, -55.0, 55.0);
	}
	else
	{
		AimOffset = 0.0;
	}
}

void UMultiplayerCharAnimInstance::GetDirection()
{
	float Value = CalculateDirection(Velocity, Character->GetActorRotation());
	Direction = Value;
}

void UMultiplayerCharAnimInstance::SetMovDirection(float DirectionValue, float SpeedValue)
{
	float LelDirection = DirectionValue;
	MoveDirection_LastUpdate = MoveDirection;
	if (SpeedValue <= 10.0)
	{
		MoveDirection = ECharMovDirection::None;
		return;
	}
	if (LelDirection >= -45.0 && LelDirection <= 45.0)
	{
		MoveDirection = ECharMovDirection::Forward;
		return;
	}
	if (LelDirection >= -135.0 && LelDirection < -45.0) 
	{
		MoveDirection = ECharMovDirection::Left;
		return;
	}
	if (LelDirection >= 45.0 && LelDirection < 135.0)
	{
		MoveDirection = ECharMovDirection::Right;
		return;
	}
	else
	{
		MoveDirection = ECharMovDirection::Backward;
		return;
	}
}

void UMultiplayerCharAnimInstance::PlayDashMontage(ECharMovDirection AnimDirection)
{
	switch (AnimDirection)
	{
	case ECharMovDirection::None:
		if (Character->DashForward)
		{
			Montage_Play(Character->DashForward);
		}
		break;
	case ECharMovDirection::Forward:
		if (Character->DashForward)
		{
			Montage_Play(Character->DashForward);
		}
		break;
	case ECharMovDirection::Backward:
		if (Character->DashBackward)
		{
			Montage_Play(Character->DashBackward);
		}
		break;
	case ECharMovDirection::Left:
		if (Character->DashLeft)
		{
			Montage_Play(Character->DashLeft);
		}
		break;
	case ECharMovDirection::Right:
		if (Character->DashRight)
		{
			Montage_Play(Character->DashRight);
		}
		break;
	}
	Character->OnAbilityVFX(false);
	return;
}
