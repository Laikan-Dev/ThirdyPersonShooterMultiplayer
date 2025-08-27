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
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* ExitedPlayer) override;
protected:
	virtual void HandleMatchHasStarted() override;
};
