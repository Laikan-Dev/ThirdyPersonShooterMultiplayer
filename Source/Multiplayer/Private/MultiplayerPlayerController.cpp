// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Multiplayer/MultiplayerCharacter.h"
#include "Kismet/GameplayStatics.h"

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

void AMultiplayerPlayerController::AddMatchResultWidget(ETeam VictoriusTeam)
{
	if (IsLocalController())
	{
		if (MatchResultWidget) 
		{
			AMultiplayerCharacter* PlayerCharacter = Cast<AMultiplayerCharacter>(GetPawn());
			UUserWidget* WidgetInstance = CreateWidget<UUserWidget>(this, MatchResultWidget);
				if (WidgetInstance && PlayerCharacter)
				{
					if (PlayerCharacter->CurrentTeam == VictoriusTeam)
					{
						MatchResult = TEXT("Victory");
						WidgetInstance->AddToViewport();
					}
					else
					{
						MatchResult = TEXT("Defeat");
						WidgetInstance->AddToViewport();
					}
					UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(this, WidgetInstance);
				}
		}
	}
}

void AMultiplayerPlayerController::BeginPlay()
{
	if (IsLocalController())
	{
		if (CaptureFlagWidget)
		{
			UUserWidget* WidgetInstance = CreateWidget<UUserWidget>(this, CaptureFlagWidget);
			if (WidgetInstance)
			{
				WidgetInstance->AddToViewport();
			}
		}
	}
}
