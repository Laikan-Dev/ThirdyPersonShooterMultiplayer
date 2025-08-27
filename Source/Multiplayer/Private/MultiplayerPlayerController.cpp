// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerPlayerController.h"
#include "Multiplayer/Enums/Announcement.h"
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
#include "CombatComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Multiplayer/GameHead/ChaosRemGameState.h"
#include "Multiplayer/HUD/ReturnToMainMenu.h"

void AMultiplayerPlayerController::CheckTimeSync(float DeltaTime)
{
	TimeSyncRunningTime += DeltaTime;
	if (IsLocalController() && TimeSyncRunningTime > TimeSyncFrequency)
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
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
				if (bInitializedHealth) SetHudHealth(HUDHealth, HUDMaxHealth);
				if (bInitializedShield) SetHudShield(HUDShield, HUDMaxShield);
				if (bInitializedScore) SetHUDScore(HUDScore);
				if (bInitializedDefeats )SetHUDDefeats(HUDDefeats);
				if (bInitializedWeaponAmmo) SetHUDWeaponAmmo(HUDWeaponAmmo);
				if (bInitializedCarriedAmmo) SetHUDCarriedAmmo(HUDCarriedAmmo);
				AMultiplayerCharacter* CharacterRef = Cast<AMultiplayerCharacter>(GetPawn());
				if (CharacterRef && CharacterRef->GetCombatSystem())
				{
					if (bInitializedGrenades) SetHUDGrenades(CharacterRef->GetCombatSystem()->GetGrenades());
				}
			}
		}
	}
}

void AMultiplayerPlayerController::HandleMatchHasStarted(bool bTeamsMatch)
{
	if (HasAuthority()) bShowTeamScores = bTeamsMatch;
	MultiplayerHUD = MultiplayerHUD == nullptr ? Cast<AMultiplayerHud>(GetHUD()) : MultiplayerHUD;
	if (MultiplayerHUD)
	{
		if (MultiplayerHUD->CharacterOverlay == nullptr) MultiplayerHUD->AddCharacterOverlay();
		if (MultiplayerHUD->Announcement)
		{
			MultiplayerHUD->Announcement->SetVisibility(ESlateVisibility::Hidden);
		}
		if (!HasAuthority()) return;
		if (bTeamsMatch)
		{
			InitTeamScores();
		}
		else
		{
			HideTeamScores();
		}
	}
}

void AMultiplayerPlayerController::ClientElimAnnouncement_Implementation(APlayerState* Attacker, APlayerState* Victim)
{
	APlayerState* Self = GetPlayerState<APlayerState>();
	if (Attacker && Victim && Self)
	{
		MultiplayerHUD = MultiplayerHUD == nullptr ? Cast<AMultiplayerHud>(GetHUD()) : MultiplayerHUD;
		if (MultiplayerHUD)
		{
			if (Attacker == Self && Victim != Self)
			{
				MultiplayerHUD->AddElimAnnouncement("You", Victim->GetPlayerName());
				return;
			}
			if (Victim == Self && Attacker != Self)
			{
				MultiplayerHUD->AddElimAnnouncement(Attacker->GetPlayerName(), "you");
				return;
			}
			if (Attacker == Victim && Attacker == Self)
			{
				MultiplayerHUD->AddElimAnnouncement("You", "yourself");
				return;
			}
			if (Attacker == Victim && Attacker != Self)
			{
				MultiplayerHUD->AddElimAnnouncement(Attacker->GetPlayerName(), "themselves");
				return;
			}
			MultiplayerHUD->AddElimAnnouncement(Attacker->GetPlayerName(), Victim->GetPlayerName());
		}
	}
}

void AMultiplayerPlayerController::BroadcastElim(APlayerState* Attacker, APlayerState* Victim)
{
	ClientElimAnnouncement(Attacker, Victim);
}

void AMultiplayerPlayerController:: ClientJoinMidGame_Implementation(FName StateMatch, float Warmup, float Match, float Cooldown,float StartingTime)
{
	WarmupTime = Warmup;
	MatchTime = Match;
	CooldownTime = Cooldown;
	LevelStartingTime = StartingTime;
	MatchState = StateMatch;
	OnMatchStateSet(MatchState);
	if (MultiplayerHUD && MatchState == MatchState::WaitingToStart)
	{
		MultiplayerHUD->AddAnnouncement();
	}
}

void AMultiplayerPlayerController::ServerCheckMatchState_Implementation()
{
	GameMode = GameMode == nullptr ? Cast<AAsTheCaosRemainsGameMode>(UGameplayStatics::GetGameMode(this)) : GameMode;
	if (GameMode)
	{
		WarmupTime = GameMode->WarmupTime;
		MatchTime = GameMode->MatchTime;
		CooldownTime = GameMode->CooldownTime;
		LevelStartingTime = GameMode->LevelStartingTime;
		MatchState = GameMode->GetMatchState();
		ClientJoinMidGame(MatchState, WarmupTime, MatchTime, CooldownTime, LevelStartingTime);
	}
}

void AMultiplayerPlayerController::OnRep_ShowTeamScores()
{
	if (bShowTeamScores)
	{
		InitTeamScores();
	}
	else
	{
		HideTeamScores();
	}
}

FString AMultiplayerPlayerController::GetInfoText(const TArray<class AChaosRemPlayerState*>& Players)
{
	AChaosRemPlayerState* ChaosRemPlayerState = GetPlayerState<AChaosRemPlayerState>();
	if (ChaosRemPlayerState == nullptr) return FString();
	FString InfoTextString;
	if (Players.Num() == 0)
	{
		InfoTextString = Annoucement::ThereIsNoWinner;
	}
	else if (Players.Num() == 1 && Players[0] == ChaosRemPlayerState)
	{
		InfoTextString = Annoucement::YouAreTheWinner;
	}
	else if (Players.Num() == 1)
	{
		InfoTextString = FString::Printf(TEXT("Winner: \n%s"), *Players[0]->GetPlayerName());
	}
	else if (Players.Num() > 1)
	{
		InfoTextString = Annoucement::PlayerTiedForTheWin;
		InfoTextString.Append(FString::Printf(TEXT("\n")));
		for (auto TiedPlayer : Players)
		{
			InfoTextString.Append(FString::Printf(TEXT("%s\n"), *TiedPlayer->GetPlayerName()));
		}
	}
	return InfoTextString;
}

FString AMultiplayerPlayerController::GetTeamsInfoText(class AChaosRemGameState* ChaosGameState)
{
	if (ChaosGameState == nullptr) return FString();
	FString InfoTextString;
	const int32 RedTeamScore = ChaosGameState->RedTeamScore;
	const int32 BlueTeamScore = ChaosGameState->BlueTeamScore;

	if (RedTeamScore == 0 && BlueTeamScore == 0)
	{
		InfoTextString = Annoucement::ThereIsNoWinner;
	}
	else if (RedTeamScore == BlueTeamScore)
	{
		InfoTextString = FString::Printf(TEXT("%s\n"), *Annoucement::TeamsTiedForTheWin);
		InfoTextString.Append(Annoucement::RedTeam);
		InfoTextString.Append(TEXT("\n"));
		InfoTextString.Append(Annoucement::BlueTeam);
		InfoTextString.Append(TEXT("\n"));
	}
	else if (RedTeamScore > BlueTeamScore)
	{
		InfoTextString = Annoucement::RedTeamWins;
		InfoTextString.Append(TEXT("\n"));
		InfoTextString.Append(FString::Printf(TEXT("%s: %d\n"), *Annoucement::RedTeam, RedTeamScore));
		InfoTextString.Append(FString::Printf(TEXT("%s: %d\n"), *Annoucement::BlueTeam, BlueTeamScore));
	}
	else if (BlueTeamScore > RedTeamScore)
	{
		InfoTextString = Annoucement::BlueTeamWins;
		InfoTextString.Append(TEXT("\n"));
		InfoTextString.Append(FString::Printf(TEXT("%s: %d\n"), *Annoucement::BlueTeam, BlueTeamScore));
		InfoTextString.Append(FString::Printf(TEXT("%s: %d\n"), *Annoucement::RedTeam, RedTeamScore));
	}
	
	return InfoTextString;
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
		bInitializedHealth = true;
		HUDHealth = CurrentHealth;
		HUDMaxHealth = MaxHealth;
	}
}

void AMultiplayerPlayerController::SetHudShield(float CurrentShield, float MaxShield)
{
	MultiplayerHUD = MultiplayerHUD == nullptr ? Cast<AMultiplayerHud>(GetHUD()) : MultiplayerHUD;
	bool bHUDValid = MultiplayerHUD && MultiplayerHUD->CharacterOverlay && MultiplayerHUD->CharacterOverlay->ShieldBar && MultiplayerHUD->CharacterOverlay->ShieldText;
	if (bHUDValid)
	{
		const float ShieldPercent = CurrentShield / MaxShield;
		MultiplayerHUD->CharacterOverlay->ShieldBar->SetPercent(ShieldPercent);
		FString ShieldText = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(CurrentShield), FMath::CeilToInt(MaxShield));
		MultiplayerHUD->CharacterOverlay->ShieldText->SetText(FText::FromString(ShieldText));
	}
	else
	{
		bInitializedShield = true;
		HUDShield = CurrentShield;
		HUDMaxShield = MaxShield;
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
		bInitializedScore = true;
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
		bInitializedDefeats = true;
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
	else
	{
		bInitializedWeaponAmmo = true;
		HUDWeaponAmmo = Ammo;
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
	else
	{
		bInitializedCarriedAmmo = true;
		HUDCarriedAmmo = Ammo;
	}
}

void AMultiplayerPlayerController::SetHUDMatchCountdown(float CountdownTime)
{
	MultiplayerHUD = MultiplayerHUD == nullptr ? Cast<AMultiplayerHud>(GetHUD()) : MultiplayerHUD;
	bool bHUDValid = MultiplayerHUD && MultiplayerHUD->CharacterOverlay && MultiplayerHUD->CharacterOverlay->TimerCountdownText;
	if (bHUDValid)
	{
		if (CountdownTime < 0.f)
		{
			MultiplayerHUD->CharacterOverlay->TimerCountdownText->SetText(FText());
			return;
		}
		int32 Minutes = FMath::FloorToInt( CountdownTime / 60.0f );
		int32 Seconds = CountdownTime - Minutes * 60.0f;
		FString CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		MultiplayerHUD->CharacterOverlay->TimerCountdownText->SetText(FText::FromString(CountdownText));
	}
}

void AMultiplayerPlayerController::SetHUDGrenades(int32 Grenades)
{
	MultiplayerHUD = MultiplayerHUD == nullptr ? Cast<AMultiplayerHud>(GetHUD()) : MultiplayerHUD;
	bool bHUDValid = MultiplayerHUD && MultiplayerHUD->CharacterOverlay && MultiplayerHUD->CharacterOverlay->GrenadesText;
	if (bHUDValid)
	{
		FString GrenadesText = FString::Printf(TEXT("%d"), Grenades);
		MultiplayerHUD->CharacterOverlay->GrenadesText->SetText(FText::FromString(GrenadesText));
	}
	else
	{
		bInitializedGrenades = true;
		HUDGrenades = Grenades;
	}
}

void AMultiplayerPlayerController::OnMatchStateSet(FName State, bool bTeamsMatch)
{
	MatchState = State;

	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted(bTeamsMatch);
	}
	else if (MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
}

void AMultiplayerPlayerController::SetHUDAnnouncementCountdown(float CountdownTime)
{
	MultiplayerHUD = MultiplayerHUD == nullptr ? Cast<AMultiplayerHud>(GetHUD()) : MultiplayerHUD;
	bool bHUDValid = MultiplayerHUD && MultiplayerHUD->Announcement && MultiplayerHUD->Announcement->WarmupTime;
	if (bHUDValid)
	{
		if (CountdownTime < 0.f)
		{
			MultiplayerHUD->Announcement->WarmupTime->SetText(FText());
			return;
		}
		int32 Minutes = FMath::FloorToInt( CountdownTime / 60.0f );
		int32 Seconds = CountdownTime - Minutes * 60.0f;
		
		FString CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		MultiplayerHUD->Announcement->WarmupTime->SetText(FText::FromString(CountdownText));
	}
}

void AMultiplayerPlayerController::HandleCooldown()
{
	MultiplayerHUD = MultiplayerHUD == nullptr ? Cast<AMultiplayerHud>(GetHUD()) : MultiplayerHUD;
	if (MultiplayerHUD)
	{
		MultiplayerHUD->CharacterOverlay->RemoveFromParent();
		bool bHUDValid = MultiplayerHUD->Announcement && MultiplayerHUD->Announcement->AnnouncementText && MultiplayerHUD->Announcement->InfoText;
		if (bHUDValid)
		{
			MultiplayerHUD->Announcement->SetVisibility(ESlateVisibility::Visible);
			FString AnnouncementText = Annoucement::NewMatchStartsIn;
			MultiplayerHUD->Announcement->AnnouncementText->SetText(FText::FromString(AnnouncementText));

			AChaosRemGameState* ChaosRemGameState = Cast<AChaosRemGameState>(UGameplayStatics::GetGameState(this));
			AChaosRemPlayerState* ChaosRemPlayerState = GetPlayerState<AChaosRemPlayerState>();
			if (ChaosRemGameState && ChaosRemPlayerState)
			{
				TArray<AChaosRemPlayerState*> TopPlayers = ChaosRemGameState->TopScoringPlayers;
				FString InfoTextString = bShowTeamScores ? GetTeamsInfoText(ChaosRemGameState) : GetInfoText(TopPlayers);
				
				MultiplayerHUD->Announcement->InfoText->SetText(FText::FromString(InfoTextString));
			}
		}
	}
	AMultiplayerCharacter* ChaosCharacter = Cast<AMultiplayerCharacter>(GetPawn());
	if (ChaosCharacter && ChaosCharacter->GetCombatSystem())
	{
		ChaosCharacter->bDisableGameplay = true;
		ChaosCharacter->GetCombatSystem()->FireButtonPressed(false);
		
	}
}

void AMultiplayerPlayerController::ShowReturnToMainMenu()
{
	if (ReturnToMainMenuWidget == nullptr) return;
	if (ReturnToMainMenu == nullptr)
	{
		ReturnToMainMenu = CreateWidget<UReturnToMainMenu>(this, ReturnToMainMenuWidget);
	}
	if (ReturnToMainMenu)
	{
		bReturnToMainMenuOpen = !bReturnToMainMenuOpen;
		if (bReturnToMainMenuOpen)
		{
			ReturnToMainMenu->MenuSetup();
		}
		else
		{
			ReturnToMainMenu->MenuTearDown();
		}
	}
}
//Teams Section
void AMultiplayerPlayerController::HideTeamScores()
{
	MultiplayerHUD = MultiplayerHUD == nullptr ? Cast<AMultiplayerHud>(GetHUD()) : MultiplayerHUD;
	if (MultiplayerHUD)
	{
		bool bHUDValid = MultiplayerHUD->CharacterOverlay && MultiplayerHUD->CharacterOverlay->RedTeamScore &&
			MultiplayerHUD->CharacterOverlay->BlueTeamScore && MultiplayerHUD->CharacterOverlay->ScoreSpacerText;
		if (bHUDValid)
		{
			MultiplayerHUD->CharacterOverlay->RedTeamScore->SetText(FText());
			MultiplayerHUD->CharacterOverlay->BlueTeamScore->SetText(FText());
			MultiplayerHUD->CharacterOverlay->ScoreSpacerText->SetText(FText());


		}
	}
}

void AMultiplayerPlayerController::InitTeamScores()
{
	MultiplayerHUD = MultiplayerHUD == nullptr ? Cast<AMultiplayerHud>(GetHUD()) : MultiplayerHUD;
	if (MultiplayerHUD)
	{
		bool bHUDValid = MultiplayerHUD->CharacterOverlay && MultiplayerHUD->CharacterOverlay->RedTeamScore &&
			MultiplayerHUD->CharacterOverlay->BlueTeamScore && MultiplayerHUD->CharacterOverlay->ScoreSpacerText;
		if (bHUDValid)
		{
			FString Zero("0");
			FString Spacer("|");
			MultiplayerHUD->CharacterOverlay->RedTeamScore->SetText(FText::FromString(Zero));
			MultiplayerHUD->CharacterOverlay->BlueTeamScore->SetText(FText::FromString(Zero));
			MultiplayerHUD->CharacterOverlay->ScoreSpacerText->SetText(FText::FromString(Spacer));
		}
	}
}

void AMultiplayerPlayerController::SetHUDTeamScores()
{
}

void AMultiplayerPlayerController::SetHUDRedTeamScore(int32 RedScore)
{
	MultiplayerHUD = MultiplayerHUD == nullptr ? Cast<AMultiplayerHud>(GetHUD()) : MultiplayerHUD;
	if (MultiplayerHUD)
	{
		bool bHUDValid = MultiplayerHUD->CharacterOverlay && MultiplayerHUD->CharacterOverlay->RedTeamScore;
		if (bHUDValid)
		{
			FString ScoreText = FString::Printf(TEXT("%d"), RedScore);
			MultiplayerHUD->CharacterOverlay->RedTeamScore->SetText(FText::FromString(ScoreText));
		}
	}
}

void AMultiplayerPlayerController::SetHUDBlueTeamScore(int32 BlueScore)
{
	MultiplayerHUD = MultiplayerHUD == nullptr ? Cast<AMultiplayerHud>(GetHUD()) : MultiplayerHUD;
	if (MultiplayerHUD)
	{
		bool bHUDValid = MultiplayerHUD->CharacterOverlay && MultiplayerHUD->CharacterOverlay->BlueTeamScore;
		if (bHUDValid)
		{
			FString ScoreText = FString::Printf(TEXT("%d"), BlueScore);
			MultiplayerHUD->CharacterOverlay->BlueTeamScore->SetText(FText::FromString(ScoreText));
		}
	}
}
//Teams Section


void AMultiplayerPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMultiplayerPlayerController, MatchState);
	DOREPLIFETIME(AMultiplayerPlayerController, bShowTeamScores);
}

float AMultiplayerPlayerController::GetServerTime()
{
	if (HasAuthority()) return GetWorld()->GetTimeSeconds();
	else return GetWorld()->GetTimeSeconds() + ClientServerDelta;
}

void AMultiplayerPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();

	if (IsLocalController())
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
	}
}

void AMultiplayerPlayerController::OnRep_MatchState()
{
	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
	else if (MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
}

void AMultiplayerPlayerController::BeginPlay()
{
	Super::BeginPlay();
	MultiplayerHUD = Cast<AMultiplayerHud>(GetHUD());
	ServerCheckMatchState();
	
	if (IsLocalController())
	{
		if (CaptureFlagWidget)
		{
			UUserWidget* WidgetInstance = CreateWidget<UUserWidget>(this, CaptureFlagWidget);
			if (WidgetInstance)
			{
				//WidgetInstance->AddToViewport();
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
		SetHudShield(PlayerCharacter->GetCurrentShield(), PlayerCharacter->GetMaxShield());
	}
}

void AMultiplayerPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SetHUDTime();
	CheckTimeSync(DeltaTime);
	PollInit();
}

void AMultiplayerPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (InputComponent == nullptr) return;
	InputComponent->BindAction("Quit", IE_Pressed, this, &AMultiplayerPlayerController::ShowReturnToMainMenu);
	
}

void AMultiplayerPlayerController::SetHUDTime()
{
	float TimeLeft = 0.f;
	if (MatchState == MatchState::WaitingToStart) TimeLeft = WarmupTime - GetServerTime() + LevelStartingTime;
	else if (MatchState == MatchState::InProgress) TimeLeft = WarmupTime + MatchTime - GetServerTime() + LevelStartingTime;
	else if (MatchState == MatchState::Cooldown) TimeLeft = CooldownTime + WarmupTime + MatchTime - GetServerTime() + LevelStartingTime;
	uint32 SecondLeft = FMath::CeilToInt(TimeLeft);
	
	if (CountdownInt != SecondLeft)
	{
		if (MatchState == MatchState::WaitingToStart || MatchState == MatchState::Cooldown)
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

void AMultiplayerPlayerController::ServerRequestServerTime_Implementation(float TimeOfClientRequest)
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