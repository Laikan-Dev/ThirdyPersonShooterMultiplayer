// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerPlayerController.h"

#include "MultiplayerHud.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Multiplayer/MultiplayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Multiplayer/HUD/CharacterOverlay.h"

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

void AMultiplayerPlayerController::SetHudHealth(float CurrentHealth, float MaxHealth)
{
	MultiplayerHUD = MultiplayerHUD ==nullptr ? Cast<AMultiplayerHud>(GetHUD()) : MultiplayerHUD;
	bool bHUDValid = MultiplayerHUD && MultiplayerHUD->CharacterOverlay && MultiplayerHUD->CharacterOverlay->HPBar && MultiplayerHUD->CharacterOverlay->HPText;
	if (bHUDValid)
	{
		const float HealthPercent = CurrentHealth / MaxHealth;
		MultiplayerHUD->CharacterOverlay->HPBar->SetPercent(HealthPercent);
		FString HealthText = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(CurrentHealth), FMath::CeilToInt(MaxHealth));
		MultiplayerHUD->CharacterOverlay->HPText->SetText(FText::FromString(HealthText));
	}
}

void AMultiplayerPlayerController::SetHUDScore(float Score)
{
	MultiplayerHUD = MultiplayerHUD == nullptr ? Cast<AMultiplayerHud>(GetHUD()) : MultiplayerHUD;
	bool bHUDValid = MultiplayerHUD && MultiplayerHUD->CharacterOverlay && MultiplayerHUD->CharacterOverlay->ScoreAmount;
	if (bHUDValid)
	{
		FString ScoreText = FString::Printf(TEXT("%d"), FMath::FloorToInt(Score));
		MultiplayerHUD->CharacterOverlay->ScoreAmount->SetText(FText::FromString(ScoreText));
	}
}

void AMultiplayerPlayerController::SetHUDDefeats(int32 Defeats)
{
	MultiplayerHUD = MultiplayerHUD == nullptr ? Cast<AMultiplayerHud>(GetHUD()) : MultiplayerHUD;
	bool bHUDValid = MultiplayerHUD && MultiplayerHUD->CharacterOverlay && MultiplayerHUD->CharacterOverlay->ScoreAmount;
	if (bHUDValid)
	{
		FString DefeatsText = FString::Printf(TEXT("%d"), Defeats);
		MultiplayerHUD->CharacterOverlay->DefeatsAmount->SetText(FText::FromString(DefeatsText));
	}
}

void AMultiplayerPlayerController::SetHUDWeaponAmmo(int32 Ammo)
{
	MultiplayerHUD = MultiplayerHUD == nullptr ? Cast<AMultiplayerHud>(GetHUD()) : MultiplayerHUD;
	bool bHUDValid = MultiplayerHUD && MultiplayerHUD->CharacterOverlay && MultiplayerHUD->CharacterOverlay->WeaponAmmoAmount;
	if (bHUDValid)
	{
		FString AmmoText = FString::Printf(TEXT("%d"), Ammo);
		MultiplayerHUD->CharacterOverlay->WeaponAmmoAmount->SetText(FText::FromString(AmmoText));
	}
}

void AMultiplayerPlayerController::SetHUDCarriedAmmo(int32 Ammo)
{
	MultiplayerHUD = MultiplayerHUD == nullptr ? Cast<AMultiplayerHud>(GetHUD()) : MultiplayerHUD;
	bool bHUDValid = MultiplayerHUD && MultiplayerHUD->CharacterOverlay && MultiplayerHUD->CharacterOverlay->CarriedAmmoAmount;
	if (bHUDValid)
	{
		FString AmmoText = FString::Printf(TEXT("%d"), Ammo);
		MultiplayerHUD->CharacterOverlay->CarriedAmmoAmount->SetText(FText::FromString(AmmoText));
	}
}

void AMultiplayerPlayerController::BeginPlay()
{
	Super::BeginPlay();
	MultiplayerHUD = Cast<AMultiplayerHud>(GetHUD());
	
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

void AMultiplayerPlayerController::OnPossess(APawn* inPawn)
{
	Super::OnPossess(inPawn);

	AMultiplayerCharacter* PlayerCharacter = Cast<AMultiplayerCharacter>(inPawn);
	if (Player)
	{
		SetHudHealth(PlayerCharacter->GetCurrentHealth(), PlayerCharacter->GetMaxHealth());
	}
}
