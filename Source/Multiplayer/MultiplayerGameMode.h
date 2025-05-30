// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/GameModeBase.h"
#include "Team.h"
#include "MultiplayerGameMode.generated.h"

UCLASS(minimalapi)
class AMultiplayerGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMultiplayerGameMode();

protected:
	virtual void BeginPlay();
};



