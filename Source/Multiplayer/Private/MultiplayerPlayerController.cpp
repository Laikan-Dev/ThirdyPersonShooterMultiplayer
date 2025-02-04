// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerPlayerController.h"
#include "Blueprint/UserWidget.h"

void AMultiplayerPlayerController::AddCaptureFlagWidget(TSubclassOf<UUserWidget> CurrentWidget)
{
	if (IsLocalController())
	{
		if (CurrentWidget)
		{
			UUserWidget* WidgetInstance = CreateWidget<UUserWidget>(this, CurrentWidget);
			if (WidgetInstance)
			{
				WidgetInstance->AddToViewport();
			}
		}
	}
}

void AMultiplayerPlayerController::BeginPlay()
{
	if (IsLocalController())
	{
		if (Widget)
		{
			UUserWidget* WidgetInstance = CreateWidget<UUserWidget>(this, Widget);
			if (WidgetInstance)
			{
				WidgetInstance->AddToViewport();
			}
		}
	}
}
