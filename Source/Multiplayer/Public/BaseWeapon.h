// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "BaseWeapon.generated.h"
USTRUCT(BlueprintType)
struct FWeaponInformation
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSubclassOf<ABaseWeapon> WeaponClass;
	UPROPERTY(EditAnywhere)
	FName Name;
	UPROPERTY(EditAnywhere)
	FString Descryption;
	UPROPERTY(EditAnywhere)
	USkeletalMesh* Mesh;
	UPROPERTY(EditAnywhere)
	UAnimationAsset* FireAnimation;
	UPROPERTY(EditAnywhere)
	UAnimationAsset* ReloadAnimation;
	UPROPERTY(EditAnywhere)
	UTexture2D* Icon;
	UPROPERTY(EditAnywhere)
	float FireRate;
	UPROPERTY(EditAnywhere)
	float Damage;
	UPROPERTY(EditAnywhere)
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