// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidgetBlueprint.h"
#include "CaptureFlagUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYER_API UCaptureFlagUserWidget : public UUserWidgetBlueprint
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 Team1ScoreValue;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 Team2ScoreValue;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 MatchTimer;
};
