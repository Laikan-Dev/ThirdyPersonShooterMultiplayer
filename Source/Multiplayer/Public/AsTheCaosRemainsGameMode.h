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
	AAsTheCaosRemainsGameMode();
	virtual void Tick(float DeltaSeconds) override;
	virtual void PlayerEliminated(class AMultiplayerCharacter* ElimmedCharacter, class AMultiplayerPlayerController* VictimController, AMultiplayerPlayerController* AttackerController);
	virtual void RequestRespawn(class ACharacter* ElimmedCharacter, AController* ElimmedController );
	UPROPERTY(EditDefaultsOnly)
	float WarmupTime = 10.f;

	UPROPERTY(EditDefaultsOnly)
	float MatchTime = 120.f;

	float LevelStartingTime;

protected:
	virtual void BeginPlay() override;
	virtual void OnMatchStateSet() override;
private:
	float CountdownTime = 0.f;
};
