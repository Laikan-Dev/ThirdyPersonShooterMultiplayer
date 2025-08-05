// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerCharAnimInstance.h"
#include "Multiplayer/MultiplayerCharacter.h"
#include "Gameframework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "BaseWeapon.h"

void UMultiplayerCharAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	Character = Cast<AMultiplayerCharacter>(TryGetPawnOwner());
	if (Character)
	{
		CharMovComponent = Character->GetCharacterMovement();
	}
}

void UMultiplayerCharAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);
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
		EquippedWeapon = Character->GetEquippedWeapon();
		bIsCrouched = Character->bIsCrouched;
		bIsAiming = Character->bIsAiming();
		TurningInPlace = Character->GetTurningInPlace();

		//YawOffset
		FRotator AimRotation = Character->GetBaseAimRotation();
		FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(Character->GetVelocity());
		FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);
		DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaRot, DeltaTime, 6.f);
		YawOffset = DeltaRotation.Yaw;

		AO_Yaw = Character->GetAO_Yaw();
		AO_Pitch = Character->GetAO_Pitch();

		
		CharacterRotation = Character->GetActorRotation();
		CharacterRotationLastFrame = CharacterRotation;
		const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
		const float Target = Delta.Yaw / DeltaTime;
		const float Interp = FMath::FInterpTo(Lean, Target, DeltaTime, 6.f);
		Lean = FMath::Clamp(Interp, -90.f, 90.f);

		//GetAimOffset();
		GetDirection();
		SetMovDirection(Direction, GroundSpeed);

		if(bWeaponEquipped && EquippedWeapon && EquippedWeapon->GetWeaponMesh() && Character->GetMesh())
		{
			LeftHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("LeftHandSocket"), ERelativeTransformSpace::RTS_World);
			FVector OutPosition;
			FRotator OutRotation;
			Character->GetMesh()->TransformToBoneSpace(FName("hand_r"), LeftHandTransform.GetLocation(), FRotator::ZeroRotator, OutPosition, OutRotation);
			LeftHandTransform.SetLocation(OutPosition);
			LeftHandTransform.SetRotation(FQuat(OutRotation));


			if (Character->IsLocallyControlled()) 
			{
				bIsLocallyControlled = true;
				FTransform RightHandTransform = Character->GetMesh()->GetSocketTransform(FName("hand_r"), ERelativeTransformSpace::RTS_World);
				FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(RightHandTransform.GetLocation(), RightHandTransform.GetLocation() + (RightHandTransform.GetLocation() - Character->GetHitTarget()));
				RightHandRot = FMath::RInterpTo(RightHandRot, LookAtRotation, DeltaTime, 30.f);
				
			}
			
			FTransform MuzzleTipTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("MuzzleFlash"), ERelativeTransformSpace::RTS_World);
			FVector MuzzleX(FRotationMatrix(MuzzleTipTransform.GetRotation().Rotator()).GetScaledAxis(EAxis::X));
			DrawDebugLine(GetWorld(), MuzzleTipTransform.GetLocation(), MuzzleTipTransform.GetLocation() + MuzzleX * 1000.0, FColor::Red, false, 0.1f, 0, 1.0f);
			DrawDebugLine(GetWorld(), MuzzleTipTransform.GetLocation(), Character->GetHitTarget(), FColor::Orange);

		}

	}
}

void UMultiplayerCharAnimInstance::GetAimOffset()
{
	if (Character->bIsAiming())
	{
		FRotator AimRotator = UKismetMathLibrary::NormalizedDeltaRotator(Character->GetBaseAimRotation(), Character->GetActorRotation());
		float Value = AimRotator.Pitch * -1.0;
		AimOffset = FMath::Clamp(Value, -90.0, 90.0);
	}
	else
	{
		AimOffset = 0.0;
	}
}


//For Dash Animations

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
