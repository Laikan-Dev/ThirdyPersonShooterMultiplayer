// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MultiplayerHud.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYER_API AMultiplayerHud : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;
	
};
