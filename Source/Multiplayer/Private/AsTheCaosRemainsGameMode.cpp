// Fill out your copyright notice in the Description page of Project Settings.


#include "AsTheCaosRemainsGameMode.h"
#include "Multiplayer/MultiplayerCharacter.h"
#include "MultiplayerPlayerController.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

void AAsTheCaosRemainsGameMode::PlayerEliminated(class AMultiplayerCharacter* ElimmedCharacter, class AMultiplayerPlayerController* VictimController, AMultiplayerPlayerController* AttackerController)
{
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
