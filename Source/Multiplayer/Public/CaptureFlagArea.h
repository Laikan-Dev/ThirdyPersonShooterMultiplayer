// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Team.h"
#include "CaptureFlagArea.generated.h"

class AMultiplayerCharacter;

UCLASS()
class MULTIPLAYER_API ACaptureFlagArea : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACaptureFlagArea();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	//Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USphereComponent* TriggerCollision;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* Flag;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UParticleSystem* VFX;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UWidgetComponent* ProgressBar;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	class UUserWidget* Widget;

protected:
	//TeamColor
	UPROPERTY(EditDefaultsOnly)
	UMaterial* BlueTeamColor;
	UPROPERTY(EditDefaultsOnly)
	UMaterial* RedTeamColor;

	//Timer
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="CaptureSettings")
	float MaxCaptureTime;
	UPROPERTY(ReplicatedUsing = OnRep_CurrentCaptureTime)
	float CurrentCaptureTime;

	//CheckBools
	UPROPERTY(EditDefaultsOnly)
	bool bCapturing;
	bool bCaptured;
	bool bContesting;

	//TeamConfig
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETeam CurrentTeam;
	UPROPERTY(VisibleAnywhere)
	TArray<AMultiplayerCharacter*> RedTeamPlayersArray;
	UPROPERTY(VisibleAnywhere)
	TArray<AMultiplayerCharacter*> BlueTeamPlayersArray;
	//ReplicateFunctions
	UFUNCTION()
	void OnRep_CurrentCaptureTime();
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void OnCaptureTimeUpdate();

public:	

	//Getter for max CaptureTime
	UFUNCTION(BlueprintPure, Category = "CaptureSettings")
	FORCEINLINE float GetMaxCaptureTime() const { return MaxCaptureTime; }

	//Getter for current CaptureTime
	UFUNCTION(BlueprintPure, Category = "CaptureSettings")
	FORCEINLINE float GetCurrentCaptureTime() const { return CurrentCaptureTime; }

	//Set Capture Time
	UFUNCTION(Server, Reliable, Category = "CaptureSettings")
	void SetCurrentCaptureTime(float CaptureTimeValue);

	//SetScore to GameState
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void AddTeamScore();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void AddProgressToWidget(float value);



	//Contest The Flag
	UFUNCTION()
	void ContestingFlagArea();

	//CheckFunction
	UFUNCTION(BlueprintPure)
	bool CanIncreseCapture(AMultiplayerCharacter* CurrentPlayer, ETeam PlayerTeam);
	UFUNCTION()
	void AddPlayerToTeamArray(AMultiplayerCharacter* CurrentPlayer, ETeam PlayerTeam);
	UFUNCTION()
	void RemovePlayerToTeamArray(AMultiplayerCharacter* CurrentPlayer, ETeam PlayerTeam);
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Collision Functions
	UFUNCTION()
	void OnOverlapCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnEndOverlapCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
