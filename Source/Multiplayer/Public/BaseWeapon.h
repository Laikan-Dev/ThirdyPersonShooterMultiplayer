// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "BaseWeapon.generated.h"
USTRUCT(BlueprintType)
struct FWeaponInformation
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ABaseWeapon> WeaponClass;
	UPROPERTY(EditDefaultsOnly)
	FName Name;
	UPROPERTY(EditDefaultsOnly)
	FString Descryption;
	UPROPERTY(EditDefaultsOnly)
	USkeletalMesh* Mesh;
	UPROPERTY(EditDefaultsOnly)
	UAnimationAsset* FireAnimation;
	UPROPERTY(EditDefaultsOnly)
	UAnimationAsset* ReloadAnimation;
	UPROPERTY(EditDefaultsOnly)
	UTexture2D* Icon;
	UPROPERTY(EditDefaultsOnly)
	float FireRate;
	UPROPERTY(EditDefaultsOnly)
	float Damage;
	UPROPERTY(EditDefaultsOnly)
	int32 AmmoCap;;
};

UCLASS()
class MULTIPLAYER_API ABaseWeapon : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABaseWeapon();
	// Struct Information
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	FWeaponInformation WeaponInformation;
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USphereComponent* SphereComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USkeletalMeshComponent* SkeletalMesh;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable)
	void PickUp(AMultiplayerCharacter* Player);

public:
	UFUNCTION()
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};