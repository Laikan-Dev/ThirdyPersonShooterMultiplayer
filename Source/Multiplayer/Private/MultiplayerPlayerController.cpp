// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerPlayerController.h"
#include "Blueprint/UserWidget.h"

void AMultiplayerPlayerController::AddCaptureFlagWidget_Implementation(TSubclassOf<UUserWidget> CurrentWidget)
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