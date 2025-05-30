// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Team.h"
#include "WidgetCaptureProgressBar.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYER_API UWidgetCaptureProgressBar : public UUserWidget
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, Category = "Components", meta = (BindWidget))
	class UProgressBar* ProgressBar;

public:
	UPROPERTY(BlueprintReadWrite)
	FLinearColor TeamColor;
	UFUNCTION(BlueprintCallable)
	void UpdateBar(float CurrentValue, float MaxValue, ETeam Team);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Contesting(bool bIsContesting);
};
