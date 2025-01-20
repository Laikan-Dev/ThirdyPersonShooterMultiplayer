// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MultiplayerGameMode.generated.h"

UENUM(BlueprintType)
enum class ETeam : uint8
{
	None UMETA(DisplayName="None"),
	Red UMETA(DisplayName="RedTeam"),
	Blue UMETA(DisplayName="BlueTeam"),
};
UCLASS(minimalapi)
class AMultiplayerGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMultiplayerGameMode();

public:
	UFUNCTION(Client, Reliable, BlueprintCallable)
	void ChooseTeam(AMultiplayerCharacter* CurrentPlayer, ETeam ChosenTeam);
};



