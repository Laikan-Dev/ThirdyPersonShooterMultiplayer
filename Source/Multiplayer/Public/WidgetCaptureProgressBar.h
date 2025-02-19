// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "CoreMinimal.h"
#include "Team.h"
#include "WidgetCaptureProgressBar.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYER_API UWidgetCaptureProgressBar : public UUserWidget
{
	GENERATED_BODY()
protected:

	UPROPERTY(EditAnywhere, Category = "Components", meta = (BindWidget))
	class UProgressBar* ProgressBar;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UWidgetAnimation* ContestingAnimationIcon;
public:

	UPROPERTY(BlueprintReadWrite)
	FLinearColor TeamColor;
	UFUNCTION(BlueprintCallable)
	void UpdateBar(float CurrentValue, float MaxValue, ETeam Team);
	UFUNCTION()
	void Contesting(bool bIsContesting);
};
