// Fill out your copyright notice in the Description page of Project Settings.


#include "TeamGameMode.h"

#include "ChaosRemGameState.h"
#include "Kismet/GameplayStatics.h"

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
