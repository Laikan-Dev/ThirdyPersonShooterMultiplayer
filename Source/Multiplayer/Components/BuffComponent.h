﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "BuffComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MULTIPLAYER_API UBuffComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UBuffComponent();
	friend class AMultiplayerCharacter;
	void Heal(float HealAmmount, float HealingTime);
	void BuffSpeed(float BuffBaseSpeed, float BuffCrouchSpeed, float BuffTime);
	void SetInitialSpeeds(float BaseSpeed, float ChrouchSpeed);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	void HealRampUp(float DelatTime);
	
private:
	UPROPERTY()
	AMultiplayerCharacter* Character;

	bool bIsHealing = false;
	float HealingRate = 0;
	float AmountToHeal = 0.f;

	//Speed
	FTimerHandle SpeedBuffTimer;
	void ResetSpeedBuffTimer();
	float InitialBaseSpeed;
	float InitialCrouchSpeed;
	UFUNCTION(NetMulticast, Reliable)
	void MulticastSpeedBuff(float BaseSpeed, float CrouchSpeed);

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
