// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameState.h"
#include "Multiplayer/Player/ChaosRemPlayerState.h"
#include "ChaosRemGameState.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYER_API AChaosRemGameState : public AGameState
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void UpdateTopScore(class AChaosRemPlayerState* ScoringPlayer);
	
	UPROPERTY(Replicated)
	TArray<AChaosRemPlayerState*> TopScoringPlayers;


	//TEAM
	void RedTeamScores();
	void BlueTeamScores();
	
	UPROPERTY()
	TArray<AChaosRemPlayerState*> RedTeam;
	UPROPERTY()
	TArray<AChaosRemPlayerState*> BlueTeam;

	UPROPERTY(ReplicatedUsing=OnRep_RedTeamScore)
	float RedTeamScore = 0.f;
	UFUNCTION()
	void OnRep_RedTeamScore();

	UPROPERTY(ReplicatedUsing=OnRep_BlueTeamScore)
	float BlueTeamScore = 0.f;

	UFUNCTION()
	void OnRep_BlueTeamScore();

	
private:
	float TopScore = 0.f;
};
