// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerPlayerController.h"

#include "MultiplayerHud.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "GameFramework/GameMode.h"
#include "Multiplayer/MultiplayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Multiplayer/HUD/Announcement.h"
#include "Multiplayer/HUD/CharacterOverlay.h"
#include "Net/UnrealNetwork.h"
#include "AsTheCaosRemainsGameMode.h"

void AMultiplayerPlayerController::CheckTimeSync(float DeltaTime)
{
	TimeSyncRunningTime += DeltaTime;
	if (IsLocalController() && TimeSyncRunningTime > TimeSyncFrequency)
	{
		ServerResquestServerTime(GetWorld()->GetTimeSeconds());
		TimeSyncFrequency = 0.f;
	}
}

void AMultiplayerPlayerController::PollInit()
{
	if (CharacterOverlay == nullptr)
	{
		if (MultiplayerHUD && MultiplayerHUD->CharacterOverlay)
		{
			CharacterOverlay = MultiplayerHUD->CharacterOverlay;
			if (CharacterOverlay)
			{
				SetHudHealth(HUDHealth, HUDMaxHealth);
				SetHUDScore(HUDScore);
				SetHUDDefeats(HUDDefeats);
			}
		}
	}
}

void AMultiplayerPlayerController::HandleMatchHasStarted()
{
	MultiplayerHUD = MultiplayerHUD == nullptr ? Cast<AMultiplayerHud>(GetHUD()) : MultiplayerHUD;
	if (MultiplayerHUD)
	{
		MultiplayerHUD->AddCharacterOverlay();
		if (MultiplayerHUD->Announcement)
		{
			MultiplayerHUD->Announcement->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void AMultiplayerPlayerController::ClientJoinMidGame_Implementation(FName StateMatch, float Warmup, float Match, float StartingTime)
{
	WarmupTime = Warmup;
	MatchTime = Match;
	LevelStartingTime = StartingTime;
	MatchState = StateMatch;
	OnMatchStateSet(MatchState);
}

void AMultiplayerPlayerController::ServerCheckMatchState_Implementation()
{
	AAsTheCaosRemainsGameMode* GameMode = Cast<AAsTheCaosRemainsGameMode>(UGameplayStatics::GetGameMode(this));
	if (GameMode)
	{
		WarmupTime = GameMode->WarmupTime;
		MatchTime = GameMode->MatchTime;
		LevelStartingTime = GameMode->LevelStartingTime;
		MatchState = GameMode->GetMatchState();
		ClientJoinMidGame(MatchState, WarmupTime, MatchTime, LevelStartingTime);

		if (MultiplayerHUD && MatchState == MatchState::WaitingToStart)
		{
			MultiplayerHUD->AddAnnouncement();
		}
	}
}

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
	else
	{
		InitializeCharacterOverlay = true;
		HUDHealth = CurrentHealth;
		HUDMaxHealth = MaxHealth;
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
	else
	{
		InitializeCharacterOverlay = true;
		HUDMaxHealth = Score;
	}
}

void AMultiplayerPlayerController::SetHUDDefeats(int32 Defeats)
{
	MultiplayerHUD = MultiplayerHUD == nullptr ? Cast<AMultiplayerHud>(GetHUD()) : MultiplayerHUD;
	bool bHUDValid = MultiplayerHUD && MultiplayerHUD->CharacterOverlay && MultiplayerHUD->CharacterOverlay->DefeatsAmount;
	if (bHUDValid)
	{
		FString DefeatsText = FString::Printf(TEXT("%d"), Defeats);
		MultiplayerHUD->CharacterOverlay->DefeatsAmount->SetText(FText::FromString(DefeatsText));
	}
	else
	{
		InitializeCharacterOverlay = true;
		HUDDefeats = Defeats;
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

void AMultiplayerPlayerController::SetHUDMatchCountdown(float CountdownTime)
{
	MultiplayerHUD = MultiplayerHUD == nullptr ? Cast<AMultiplayerHud>(GetHUD()) : MultiplayerHUD;
	bool bHUDValid = MultiplayerHUD && MultiplayerHUD->CharacterOverlay && MultiplayerHUD->CharacterOverlay->TimerCountdownText;
	if (bHUDValid)
	{
		int32 Minutes = FMath::FloorToInt( CountdownTime / 60.0f );
		int32 Seconds = CountdownTime - Minutes * 60.0f;
		FString CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		MultiplayerHUD->CharacterOverlay->TimerCountdownText->SetText(FText::FromString(CountdownText));
	}
}

void AMultiplayerPlayerController::OnMatchStateSet(FName State)
{
	MatchState = State;

	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
}

void AMultiplayerPlayerController::SetHUDAnnouncementCountdown(float CountdownTime)
{
	MultiplayerHUD = MultiplayerHUD == nullptr ? Cast<AMultiplayerHud>(GetHUD()) : MultiplayerHUD;
	bool bHUDValid = MultiplayerHUD && MultiplayerHUD->Announcement && MultiplayerHUD->Announcement->WarmupTime;
	if (bHUDValid)
	{
		int32 Minutes = FMath::FloorToInt( CountdownTime / 60.0f );
		int32 Seconds = CountdownTime - Minutes * 60.0f;
		
		FString CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		MultiplayerHUD->Announcement->WarmupTime->SetText(FText::FromString(CountdownText));
	}
}

void AMultiplayerPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMultiplayerPlayerController, MatchState);
}

float AMultiplayerPlayerController::GetServerTime()
{
	return GetWorld()->GetTimeSeconds() + ClientServerDelta;
}

void AMultiplayerPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();

	if (IsLocalController())
	{
		ServerResquestServerTime(GetWorld()->GetTimeSeconds());
	}
}

void AMultiplayerPlayerController::OnRep_MatchState()
{
	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
}

void AMultiplayerPlayerController::BeginPlay()
{
	Super::BeginPlay();
	ServerCheckMatchState();
	
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

void AMultiplayerPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SetHUDTime();
	CheckTimeSync(DeltaTime);
	PollInit();
}

void AMultiplayerPlayerController::SetHUDTime()
{
	float TimeLeft = 0.f;
	if (MatchState == MatchState::WaitingToStart) TimeLeft = WarmupTime - GetServerTime() + LevelStartingTime;
	else if (MatchState == MatchState::InProgress) TimeLeft = WarmupTime + MatchTime - GetServerTime() + LevelStartingTime;
	uint32 SecondLeft = FMath::CeilToInt(MatchTime - GetServerTime());
	if (CountdownInt != SecondLeft)
	{
		if (MatchState == MatchState::WaitingToStart)
		{
			SetHUDAnnouncementCountdown(TimeLeft);
		}
		if (MatchState == MatchState::InProgress)
		{
			SetHUDMatchCountdown(TimeLeft);
		}
	}
	CountdownInt = SecondLeft;
}

void AMultiplayerPlayerController::ServerResquestServerTime_Implementation(float TimeOfClientRequest)
{
	float ServerTimeOfReceipt = GetWorld()->GetTimeSeconds();
	ClientReportServerTime(TimeOfClientRequest, ServerTimeOfReceipt);
}

void AMultiplayerPlayerController::ClientReportServerTime_Implementation(float TimeOfClientRequest,
	float TimeServerReceivedClientRequest)
{
	float RoundTripTime = GetWorld()->GetTimeSeconds() - TimeOfClientRequest;
	float CurrentServerTime = TimeServerReceivedClientRequest + (0.5f * RoundTripTime);
	ClientServerDelta = CurrentServerTime - GetWorld()->GetTimeSeconds();
}


