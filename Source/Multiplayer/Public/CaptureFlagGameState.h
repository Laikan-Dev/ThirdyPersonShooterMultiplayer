// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "CaptureFlagGameState.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYER_API ACaptureFlagGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	ACaptureFlagGameState();

protected:

	

	virtual void BeginPlay();
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UPROPERTY(ReplicatedUsing = OnRep_TimeLeft, BlueprintReadOnly, Category = "Game")
	int32 TimeLeft;
	UPROPERTY(ReplicatedUsing = OnRep_TimeLeft, BlueprintReadWrite, Category = "Game")
	int32 BlueTeamScore;
	UPROPERTY(ReplicatedUsing = OnRep_TimeLeft, BlueprintReadWrite, Category = "Game")
	int32 RedTeamScore;
	
	UFUNCTION()
	TArray<AMultiplayerPlayerController*> GetAllPlayerController();

	UFUNCTION()
	void OnRep_TimeLeft();
	UFUNCTION()
	void OnRep_TeamScore();
	UFUNCTION()
	void SetTimeLeft(int32 NewTime);
	UFUNCTION(BlueprintCallable)
	void SetScore(ETeam Team, int32 NewScore);
	UFUNCTION()
	void UpdateTimer();

};
