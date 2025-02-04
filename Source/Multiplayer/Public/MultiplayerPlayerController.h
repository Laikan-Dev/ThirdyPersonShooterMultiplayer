// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MultiplayerPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYER_API AMultiplayerPlayerController : public APlayerController
{
	GENERATED_BODY()


public:

	virtual void BeginPlay();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UUserWidget> CaptureFlagWidget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UUserWidget> MatchResultWidget;
	UPROPERTY(BlueprintReadWrite, Category = "Game")
	FString MatchResult;


	UFUNCTION()
	void AddCaptureFlagWidget(TSubclassOf<UUserWidget> CurrentWidget);
	UFUNCTION()
	void AddMatchResultWidget(ETeam VictoriusTeam);
	
};
