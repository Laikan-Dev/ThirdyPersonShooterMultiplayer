// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "AsTheCaosRemainsGameMode.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYER_API AAsTheCaosRemainsGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	virtual void PlayerEliminated(class AMultiplayerCharacter* ElimmedCharacter, class AMultiplayerPlayerController* VictimController, AMultiplayerPlayerController* AttackerController);
	virtual void RequestRespawn(class ACharacter* ElimmedCharacter, AController* ElimmedController );
};
