// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerInfoWidget.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYER_API UPlayerInfoWidget : public UUserWidget
{
	GENERATED_BODY()


public:

	UPROPERTY(EditAnywhere, Category = "Components", meta = (BindWidget))
	class UProgressBar* HealthBar;

	UFUNCTION()
	void UpdateBar(float Value);
};
