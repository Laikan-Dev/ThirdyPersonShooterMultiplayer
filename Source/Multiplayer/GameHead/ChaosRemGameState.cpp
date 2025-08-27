// Fill out your copyright notice in the Description page of Project Settings.


#include "ChaosRemGameState.h"

#include "MultiplayerPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "Multiplayer/Player/ChaosRemPlayerState.h"

void AChaosRemGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AChaosRemGameState, TopScoringPlayers);
	DOREPLIFETIME(AChaosRemGameState, RedTeamScore);
	DOREPLIFETIME(AChaosRemGameState, BlueTeamScore);
}

void AChaosRemGameState::UpdateTopScore(class AChaosRemPlayerState* ScoringPlayer)
{
	if (TopScoringPlayers.Num() == 0)
	{
		TopScoringPlayers.Add(ScoringPlayer);
		TopScore = ScoringPlayer->GetScore();
	}
	else if (ScoringPlayer->GetScore() == TopScore)
	{
		TopScoringPlayers.AddUnique(ScoringPlayer);
	}
	else if (ScoringPlayer->GetScore() > TopScore)
	{
		TopScoringPlayers.Empty();
		TopScoringPlayers.AddUnique(ScoringPlayer);
		TopScore = ScoringPlayer->GetScore();
	}
}

void AChaosRemGameState::RedTeamScores()
{
	++RedTeamScore;
	AMultiplayerPlayerController* PlayerController = Cast<AMultiplayerPlayerController>(GetWorld()->GetFirstPlayerController());
	if (PlayerController)
	{
		PlayerController->SetHUDRedTeamScore(RedTeamScore);
	}
}

void AChaosRemGameState::BlueTeamScores()
{
	++BlueTeamScore;
	AMultiplayerPlayerController* PlayerController = Cast<AMultiplayerPlayerController>(GetWorld()->GetFirstPlayerController());
	if (PlayerController)
	{
		PlayerController->SetHUDBlueTeamScore(BlueTeamScore);
	}
}

void AChaosRemGameState::OnRep_RedTeamScore()
{
	AMultiplayerPlayerController* PlayerController = Cast<AMultiplayerPlayerController>(GetWorld()->GetFirstPlayerController());
	if (PlayerController)
	{
		PlayerController->SetHUDRedTeamScore(RedTeamScore);
	}
}

void AChaosRemGameState::OnRep_BlueTeamScore()
{
	AMultiplayerPlayerController* PlayerController = Cast<AMultiplayerPlayerController>(GetWorld()->GetFirstPlayerController());
	if (PlayerController)
	{
		PlayerController->SetHUDBlueTeamScore(BlueTeamScore);
	}
}
