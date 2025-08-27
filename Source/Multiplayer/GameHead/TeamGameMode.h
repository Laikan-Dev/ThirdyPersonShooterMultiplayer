// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "../Public/AsTheCaosRemainsGameMode.h"
#include "TeamGameMode.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYER_API ATeamGameMode : public AAsTheCaosRemainsGameMode
{
	GENERATED_BODY()

public:
	ATeamGameMode();
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* ExitedPlayer) override;
	virtual void PlayerEliminated(class AMultiplayerCharacter* ElimmedCharacter, class AMultiplayerPlayerController* VictimController, AMultiplayerPlayerController* AttackerController) override;
	virtual float CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage) override;
protected:
	virtual void HandleMatchHasStarted() override;
};
