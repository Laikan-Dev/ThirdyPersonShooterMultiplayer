// Fill out your copyright notice in the Description page of Project Settings.


#include "AsTheCaosRemainsGameMode.h"
#include "Multiplayer/MultiplayerCharacter.h"
#include "MultiplayerPlayerController.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Multiplayer/Player/ChaosRemPlayerState.h"
#include "Multiplayer/GameHead/ChaosRemGameState.h"

namespace MatchState
{
	const FName Cooldown = FName("Cooldown");
}
AAsTheCaosRemainsGameMode::AAsTheCaosRemainsGameMode()
{
	bDelayedStart = true;
	
}
void AAsTheCaosRemainsGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	LevelStartingTime = GetWorld()->GetTimeSeconds();
}

void AAsTheCaosRemainsGameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		AMultiplayerPlayerController* PlayerController = Cast<AMultiplayerPlayerController>(*It);
		if (PlayerController)
		{
			PlayerController->OnMatchStateSet(MatchState, bTeamsMatch);
		}
	}
}

void AAsTheCaosRemainsGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (MatchState == MatchState::WaitingToStart)
	{
		CountdownTime = WarmupTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountdownTime <= 0.f)
		{
			StartMatch();
		}
	}
	else if (MatchState == MatchState::InProgress)
		{
			CountdownTime = WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
			if (CountdownTime <= 0.f)
    			{
    				SetMatchState(MatchState::Cooldown);
    			}
		}
	else if (MatchState == MatchState::Cooldown)
		{
			CountdownTime = CooldownTime + WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
			if (CountdownTime <= 0.f)
				{
    				RestartGame();
    			}
		}
}

void AAsTheCaosRemainsGameMode::PlayerEliminated(class AMultiplayerCharacter* ElimmedCharacter, class AMultiplayerPlayerController* VictimController, AMultiplayerPlayerController* AttackerController)
{
	if (AttackerController == nullptr || AttackerController->PlayerState == nullptr) return;
	if (VictimController == nullptr || VictimController->PlayerState == nullptr) return;
	AChaosRemPlayerState* AttackerPlayerState = AttackerController ? Cast<AChaosRemPlayerState>(AttackerController->PlayerState) : nullptr;
	AChaosRemPlayerState* VictimPlayerState = VictimController ? Cast<AChaosRemPlayerState>(VictimController->PlayerState) : nullptr;
	AChaosRemGameState* ChaosRemGameState = GetGameState<AChaosRemGameState>();
	if (AttackerPlayerState && AttackerPlayerState != VictimPlayerState && ChaosRemGameState)
	{
		TArray<AChaosRemPlayerState*> PlayerCurrentlyInTheLead;
		for (auto LeadPlayer : ChaosRemGameState->TopScoringPlayers)
		{
			PlayerCurrentlyInTheLead.Add(LeadPlayer);
		}
		AttackerPlayerState->AddToScore(1.f);
		ChaosRemGameState->UpdateTopScore(AttackerPlayerState);
		if (ChaosRemGameState->TopScoringPlayers.Contains(AttackerPlayerState))
		{
			AMultiplayerCharacter* Leader = Cast<AMultiplayerCharacter>(AttackerPlayerState->GetPawn());
			if (Leader)
			{
				Leader->MulticastGainedTheLead();
			}
		}
		for (int32 i = 0; i < PlayerCurrentlyInTheLead.Num(); i++)
		{
			if (!ChaosRemGameState->TopScoringPlayers.Contains(PlayerCurrentlyInTheLead[i]))
			{
				AMultiplayerCharacter* Loser = Cast<AMultiplayerCharacter>(PlayerCurrentlyInTheLead[i]->GetPawn());
				if (Loser)
				{
					Loser->MulticastLostTheLead();
				}
			}
		}
	}
	if (VictimPlayerState)
	{
		VictimPlayerState->AddToDefeats(1);
	}
	if (ElimmedCharacter)
	{
		ElimmedCharacter->Elim(false);
	}
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		AMultiplayerPlayerController* PlayerController = Cast<AMultiplayerPlayerController>(*It);
		if (PlayerController && AttackerPlayerState && VictimPlayerState)
		{
			PlayerController->BroadcastElim(AttackerPlayerState, VictimPlayerState);
		}
	}
}

void AAsTheCaosRemainsGameMode::RequestRespawn(class ACharacter* ElimmedCharacter, AController* ElimmedController)
{
	if (ElimmedCharacter)
	{
		ElimmedCharacter->Reset();
		ElimmedCharacter->Destroy();
	}
	if (ElimmedController)
	{
		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
		int32 Selection = FMath::RandRange(0, PlayerStarts.Num() - 1);
		RestartPlayerAtPlayerStart(ElimmedController, PlayerStarts[Selection]);
	}
}

void AAsTheCaosRemainsGameMode::PlayerLeftGame(AChaosRemPlayerState* PlayerLeaving)
{
	if (PlayerLeaving == nullptr) return;
	AChaosRemGameState* ChaosRemGameState = GetGameState<AChaosRemGameState>();
	if (ChaosRemGameState && ChaosRemGameState->TopScoringPlayers.Contains(PlayerLeaving))
	{
		ChaosRemGameState->TopScoringPlayers.Remove(PlayerLeaving);
	}
	AMultiplayerCharacter* CharacterLeaving = Cast<AMultiplayerCharacter>(PlayerLeaving->GetPawn());
	if (CharacterLeaving)
	{
		CharacterLeaving->Elim(true);
	}
}

float AAsTheCaosRemainsGameMode::CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage)
{
	return BaseDamage;
}
