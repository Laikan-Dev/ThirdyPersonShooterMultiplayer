// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MultiplayerPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYER_API AMultiplayerPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay();
	virtual void OnPossess(APawn *inPawn) override;
	virtual void Tick(float DeltaTime) override;
	void SetHUDTime();
	UFUNCTION(Server, Reliable)
	void ServerRequestServerTime(float TimeOfClientRequest);
	UFUNCTION(Client, Reliable)
	void ClientReportServerTime(float TimeOfClientRequest, float TimeServerReceivedClientRequest);

	float ClientServerDelta = 0.f;
	UPROPERTY(EditAnywhere)
	float TimeSyncFrequency = 5.f;

	float TimeSyncRunningTime = 0.f;
	void CheckTimeSync(float DeltaTime);
	void PollInit();
	void HandleMatchHasStarted();

	UFUNCTION(Server, Reliable)
	void ServerCheckMatchState();

	UFUNCTION(Client, Reliable)
	void ClientJoinMidGame(FName StateMatch, float Warmup, float Match, float Cooldown,float StartingTime);

public:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UUserWidget> CaptureFlagWidget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UUserWidget> MatchResultWidget;
	UPROPERTY(BlueprintReadWrite, Category = "Game")
	FString MatchResult;


	UFUNCTION()
	void AddCaptureFlagWidget(TSubclassOf<UUserWidget> CurrentWidget);
	UFUNCTION()
	void AddMatchResultWidget(ETeam VictoriusTeam);

	void SetHudHealth(float CurrentHealth, float MaxHealth);
	void SetHUDScore(float Score);
	void SetHUDDefeats(int32 Defeats);
	void SetHUDWeaponAmmo(int32 Ammo);
	void SetHUDCarriedAmmo(int32 Ammo);
	void SetHUDMatchCountdown(float CountdownTime);
	void SetHUDGrenades(int32 Grenades);
	void OnMatchStateSet(FName State);
	void SetHUDAnnouncementCountdown(float CountdownTime);
	void HandleCooldown();
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	virtual float GetServerTime();
	virtual void ReceivedPlayer() override;

private:
	UPROPERTY()
	class AMultiplayerHud* MultiplayerHUD;

	UPROPERTY()
	class AAsTheCaosRemainsGameMode* GameMode;

	float LevelStartingTime = 0.f;
	float MatchTime = 0.f;
	float WarmupTime = 0.f;
	float CooldownTime = 0.f;
	uint32 CountdownInt = 0;

	UPROPERTY(ReplicatedUsing = OnRep_MatchState)
	FName MatchState;

	UFUNCTION()
	void OnRep_MatchState();

	UPROPERTY()
	class UCharacterOverlay* CharacterOverlay;
	bool InitializeCharacterOverlay = false;

	float HUDHealth;
	float HUDMaxHealth;
	float HUDScore;
	int32 HUDDefeats;
	int32 HUDGrenades;
	

	
};
