// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerState.h"
#include "ChaosRemPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYER_API AChaosRemPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnRep_Score() override;
	void AddToScore(float ScoreAmount);
	void AddToDefeats(float DefeatsAmount);
	UFUNCTION()
	void OnRep_Defeats();
	
private:
	UPROPERTY()
	class AMultiplayerCharacter* PlayerCharacter;
	UPROPERTY()
	class AMultiplayerPlayerController* PlayerController;
	UPROPERTY(ReplicatedUsing = OnRep_Defeats)
	int32 Defeats;
};
