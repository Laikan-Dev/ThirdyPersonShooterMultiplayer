// Fill out your copyright notice in the Description page of Project Settings.


#include "ChaosRemPlayerState.h"
#include "MultiplayerPlayerController.h"
#include "Multiplayer/MultiplayerCharacter.h"
#include "Net/UnrealNetwork.h"

void AChaosRemPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AChaosRemPlayerState, Defeats);
}

void AChaosRemPlayerState::OnRep_Score()
{
	Super::OnRep_Score();
	PlayerCharacter = PlayerCharacter == nullptr ? Cast<AMultiplayerCharacter>(GetPawn()) : PlayerCharacter;
	if (PlayerCharacter && PlayerCharacter->Controller)
	{
		PlayerController = PlayerController == nullptr ? Cast<AMultiplayerPlayerController>(PlayerCharacter->Controller) : PlayerController;
		if (PlayerController)
		{
			PlayerController->SetHUDScore(GetScore());
		}
	}
}

void AChaosRemPlayerState::AddToScore(float ScoreAmount)
{
	SetScore(GetScore() + ScoreAmount);
	PlayerCharacter = PlayerCharacter == nullptr ? Cast<AMultiplayerCharacter>(GetPawn()) : PlayerCharacter;
	if (PlayerCharacter)
	{
		PlayerController = PlayerController == nullptr ? Cast<AMultiplayerPlayerController>(PlayerCharacter->Controller) : PlayerController;
		if (PlayerController)
		{
			PlayerController->SetHUDScore(GetScore());
		}
	}
}

void AChaosRemPlayerState::AddToDefeats(int32 DefeatsAmount)
{
	Defeats += DefeatsAmount;
	PlayerCharacter = PlayerCharacter == nullptr ? Cast<AMultiplayerCharacter>(GetPawn()) : PlayerCharacter;
	if (PlayerCharacter)
	{
		PlayerController = PlayerController == nullptr ? Cast<AMultiplayerPlayerController>(PlayerCharacter->Controller) : PlayerController;
		if (PlayerController)
		{
			PlayerController->SetHUDDefeats(Defeats);
		}
	}
}

void AChaosRemPlayerState::OnRep_Defeats()
{
	PlayerCharacter = PlayerCharacter == nullptr ? Cast<AMultiplayerCharacter>(GetPawn()) : PlayerCharacter;
	if (PlayerCharacter && PlayerCharacter->Controller)
	{
		PlayerController = PlayerController == nullptr ? Cast<AMultiplayerPlayerController>(PlayerCharacter->Controller) : PlayerController;
		if (PlayerController)
		{
			PlayerController->SetHUDDefeats(Defeats);
		}
	}
}
