// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "BaseWeapon.generated.h"

USTRUCT(BlueprintType)
struct FWeaponInformation
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ABaseWeapon> WeaponClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Descryption;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMesh* Mesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimationAsset* FireAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* ReloadAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Icon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FireRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Damage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 AmmoCap;;
};

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Initial UMETA(DisplayName = "Initial State"),
	EWS_Equipped UMETA(DisplayName = "Equipped"),
	EWS_Dropped UMETA(DisplayName = "Dropped"),
	EWS_MAX UMETA(DisplayName = "DefaultMAX"),
};

UCLASS()
class MULTIPLAYER_API ABaseWeapon : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABaseWeapon();
	UAnimationAsset* ShootingAnim;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Struct Information
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties")
	FWeaponInformation WeaponInformation;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class UWidgetComponent* PickupWidget;

	void ShowPickupWidget(bool bShowWidget);
private:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	class USphereComponent* SphereComponent;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class USkeletalMeshComponent* SkeletalMesh;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_WeaponState, Category = "Weapon Properties")
	EWeaponState WeaponState;

	UFUNCTION()
	void OnRep_WeaponState();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable)
	void PickUp(AMultiplayerCharacter* Player);

public:
	void SetWeaponState(EWeaponState State);
	FORCEINLINE USphereComponent* GetAreaSphere() const { return SphereComponent; }
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return SkeletalMesh; }

public:

	virtual void Fire(const FVector& HitTarget);

	UFUNCTION()
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};