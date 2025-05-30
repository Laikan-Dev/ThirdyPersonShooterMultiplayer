// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Team.h"
#include "AbilitySystemInterface.h"
#include "MultiplayerPlayerState.generated.h"

class UMultiplayerASComponent;
/**
 * 
 */
UCLASS()
class MULTIPLAYER_API AMultiplayerPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

protected:
	AMultiplayerPlayerState();
	virtual void BeginPlay();
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UPROPERTY(ReplicatedUsing = OnRep_TimeLeft, BlueprintReadOnly, Category = "Game")
	int32 TimeLeft;
	UPROPERTY(ReplicatedUsing = OnRep_TimeLeft, BlueprintReadOnly, Category = "Game")
	int32 BlueTeamScore;
	UPROPERTY(ReplicatedUsing = OnRep_TimeLeft, BlueprintReadOnly, Category = "Game")
	int32 RedTeamScore;

	UFUNCTION()
	void OnRep_TimeLeft();
	UFUNCTION()
	void OnRep_TeamScore();
	UFUNCTION()
	void SetTimeLeft(int32 NewTime);
	UFUNCTION()
	void SetScore(ETeam Team, int32 NewScore);
	UFUNCTION()
	void UpdateTimer();

protected:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
public:
	UPROPERTY()
	TObjectPtr<UMultiplayerASComponent> AbilitySystemComponent;

};
