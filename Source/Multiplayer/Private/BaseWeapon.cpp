// Fill out your copyright notice in the Description page of Project Settings.
#include "BaseWeapon.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Multiplayer/MultiplayerCharacter.h"
#include "WeaponInformation.h"
#include "Multiplayer/MultiplayerGameMode.h"

// Sets default values
ABaseWeapon::ABaseWeapon()
{
	bReplicates = true;
	
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ABaseWeapon::OnComponentBeginOverlap);
	SphereComponent->OnComponentEndOverlap.AddDynamic(this, &ABaseWeapon::OnComponentEndOverlap);
	RootComponent = SphereComponent;
	
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	StaticMesh->SetupAttachment(RootComponent);
	StaticMesh->SetStaticMesh(WeaponInformation.Mesh);
	WeaponInformation.BulletSpawnLoc = (StaticMesh->GetSocketLocation("FireSocket"));
}

// Called when the game starts or when spawned
void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}
void ABaseWeapon::PickUp_Implementation(AMultiplayerCharacter* Player)
{
	if (Player)
	{
		Player->SetCurrentWeapon(WeaponInformation);
		Destroy();
	}
	
}

void ABaseWeapon::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMultiplayerCharacter* Player = Cast<AMultiplayerCharacter>(OtherActor);
	if (Player)
	{
		PickUp(Player);
	}
}

void ABaseWeapon::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}



