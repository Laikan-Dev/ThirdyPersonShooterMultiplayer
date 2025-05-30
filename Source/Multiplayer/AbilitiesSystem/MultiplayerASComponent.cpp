// Fill out your copyright notice in the Description page of Project Settings.


#include "Multiplayer/AbilitiesSystem/MultiplayerASComponent.h"

UMultiplayerASComponent::UMultiplayerASComponent()
{
	SetIsReplicated(true);
}

void UMultiplayerASComponent::BeginPlay()
{
	Super::BeginPlay();
}
