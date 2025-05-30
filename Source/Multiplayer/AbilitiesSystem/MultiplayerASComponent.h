// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "MultiplayerASComponent.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYER_API UMultiplayerASComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
	
public:
	UMultiplayerASComponent();

protected:

	virtual void BeginPlay() override;
};
