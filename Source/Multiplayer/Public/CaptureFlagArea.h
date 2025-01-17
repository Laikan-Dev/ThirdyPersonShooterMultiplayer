// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CaptureFlagArea.generated.h"

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USphereComponent* TriggerCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* Flag;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UParticleSystem* VFX;

protected:
	UPROPERTY(EditDefaultsOnly, Category="CaptureSettings")
	float MaxCaptureTime;
	UPROPERTY(ReplicatedUsing = OnRep_CurrentCaptureTime)
	float CurrentCaptureTime;

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

	UFUNCTION(BlueprintCallable, Category = "CaptureSettings")
	void SetCurrentCaptureTime(float CaptureTimeValue);
	

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnOverlapCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnEndOverlapCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);



};
