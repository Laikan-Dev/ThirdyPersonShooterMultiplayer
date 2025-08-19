// Fill out your copyright notice in the Description page of Project Settings.


#include "AsTheCaosRemainsGameMode.h"
#include "Multiplayer/MultiplayerCharacter.h"
#include "MultiplayerPlayerController.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Multiplayer/Player/ChaosRemPlayerState.h"

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

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
	{
		AMultiplayerPlayerController* PlayerController = Cast<AMultiplayerPlayerController>(*It);
		if (PlayerController)
		{
			PlayerController->OnMatchStateSet(MatchState);
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
}

void AAsTheCaosRemainsGameMode::PlayerEliminated(class AMultiplayerCharacter* ElimmedCharacter, class AMultiplayerPlayerController* VictimController, AMultiplayerPlayerController* AttackerController)
{
	if (AttackerController == nullptr || AttackerController->PlayerState == nullptr) return;
	if (VictimController == nullptr || VictimController->PlayerState == nullptr) return;
	AChaosRemPlayerState* AttackerPlayerState = AttackerController ? Cast<AChaosRemPlayerState>(AttackerController->PlayerState) : nullptr;
	AChaosRemPlayerState* VictimPlayerState = VictimController ? Cast<AChaosRemPlayerState>(VictimController->PlayerState) : nullptr;

	if (AttackerPlayerState && AttackerPlayerState != VictimPlayerState)
	{
		AttackerPlayerState->AddToScore(1.f);
	}
	if (VictimPlayerState)
	{
		VictimPlayerState->AddToDefeats(1);
	}
	if (ElimmedCharacter)
	{
		ElimmedCharacter->Elim();
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
