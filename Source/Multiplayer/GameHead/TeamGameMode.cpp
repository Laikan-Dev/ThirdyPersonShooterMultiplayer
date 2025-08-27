// Fill out your copyright notice in the Description page of Project Settings.


#include "TeamGameMode.h"

#include "ChaosRemGameState.h"
#include "MultiplayerPlayerController.h"
#include "Kismet/GameplayStatics.h"

ATeamGameMode::ATeamGameMode()
{
	bTeamsMatch = true;
}

void ATeamGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	AChaosRemGameState* ChaosGameState = Cast<AChaosRemGameState>(UGameplayStatics::GetGameState(this));
	if (ChaosGameState)
	{
		AChaosRemPlayerState* ChaosPlayerState = NewPlayer->GetPlayerState<AChaosRemPlayerState>();
		if (ChaosPlayerState && ChaosPlayerState->GetTeam() == ETeam::ET_NoTeam)
		{
			if (ChaosGameState->BlueTeam.Num() >= ChaosGameState->RedTeam.Num())
			{
				ChaosGameState->RedTeam.AddUnique(ChaosPlayerState);
				ChaosPlayerState->SetTeam(ETeam::ET_RedTeam);
			}
			else
			{
				ChaosGameState->BlueTeam.AddUnique(ChaosPlayerState);
				ChaosPlayerState->SetTeam(ETeam::ET_BlueTeam);
			}
		}
	}
}

void ATeamGameMode::Logout(AController* ExitedPlayer)
{
	Super::Logout(ExitedPlayer);
	AChaosRemGameState* ChaosGameState = Cast<AChaosRemGameState>(UGameplayStatics::GetGameState(this));
	AChaosRemPlayerState* ChaosPlayerState = ExitedPlayer->GetPlayerState<AChaosRemPlayerState>();
	if (ChaosGameState && ChaosPlayerState)
	{
		if (ChaosGameState->RedTeam.Contains(ChaosPlayerState))
		{
			ChaosGameState->RedTeam.Remove(ChaosPlayerState);
		}
		if (ChaosGameState->BlueTeam.Contains(ChaosPlayerState))
		{
			ChaosGameState->BlueTeam.Remove(ChaosPlayerState);
		}
	}
}

void ATeamGameMode::PlayerEliminated(class AMultiplayerCharacter* ElimmedCharacter,
	class AMultiplayerPlayerController* VictimController, AMultiplayerPlayerController* AttackerController)
{
	Super::PlayerEliminated(ElimmedCharacter, VictimController, AttackerController);
	AChaosRemGameState* ChaosGameState = Cast<AChaosRemGameState>(UGameplayStatics::GetGameState(this));
	AChaosRemPlayerState* AttackerPlayerState = AttackerController ? Cast<AChaosRemPlayerState>(AttackerController->PlayerState) : nullptr;
	if (ChaosGameState && AttackerPlayerState)
	{
		if (AttackerPlayerState->GetTeam() == ETeam::ET_BlueTeam)
		{
			ChaosGameState->BlueTeamScores();
		}
		if (AttackerPlayerState->GetTeam() == ETeam::ET_RedTeam)
		{
			ChaosGameState->RedTeamScores();
		}
	}

	
}

float ATeamGameMode::CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage)
{
	AChaosRemPlayerState* AttackerPState = Attacker->GetPlayerState<AChaosRemPlayerState>();
	AChaosRemPlayerState* VictimPState = Victim->GetPlayerState<AChaosRemPlayerState>();

	if (AttackerPState == nullptr || VictimPState == nullptr) return BaseDamage;
	if (VictimPState == AttackerPState) return BaseDamage;
	if (AttackerPState->GetTeam() == VictimPState->GetTeam()) return 0.f;
	
	return BaseDamage;
}

void ATeamGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();

	AChaosRemGameState* ChaosGameState = Cast<AChaosRemGameState>(UGameplayStatics::GetGameState(this));
	if (ChaosGameState)
	{
		for (auto PState : ChaosGameState->PlayerArray)
		{
			AChaosRemPlayerState* ChaosPlayerState = Cast<AChaosRemPlayerState>(PState.Get());
			if (ChaosPlayerState && ChaosPlayerState->GetTeam() == ETeam::ET_NoTeam)
			{
				if (ChaosGameState->BlueTeam.Num() >= ChaosGameState->RedTeam.Num())
				{
					ChaosGameState->RedTeam.AddUnique(ChaosPlayerState);
					ChaosPlayerState->SetTeam(ETeam::ET_RedTeam);
				}
				else
				{
					ChaosGameState->BlueTeam.AddUnique(ChaosPlayerState);
					ChaosPlayerState->SetTeam(ETeam::ET_BlueTeam);
				}
			}
		}
	}
}
