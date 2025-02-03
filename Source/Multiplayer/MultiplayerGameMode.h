// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Team.h"
#include "MultiplayerGameMode.generated.h"

UCLASS(minimalapi)
class AMultiplayerGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMultiplayerGameMode();

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UUserWidget> CaptureFlagWidget;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastScoreWidgetSpawn(TSubclassOf<UUserWidget> WidgetClass);

protected:
	virtual void BeginPlay();
};



