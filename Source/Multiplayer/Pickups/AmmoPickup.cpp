// Fill out your copyright notice in the Description page of Project Settings.


#include "AmmoPickup.h"
#include "Multiplayer/MultiplayerCharacter.h"
#include "CombatComponent.h"

// Sets default values
AAmmoPickup::AAmmoPickup()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AAmmoPickup::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAmmoPickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	AMultiplayerCharacter* PlayerCharacter = Cast<AMultiplayerCharacter>(OtherActor);
	if (PlayerCharacter)
	{
		UCombatComponent* CombatSystem = PlayerCharacter->GetCombatSystem();
		if (CombatSystem)
		{
			CombatSystem->PickupAmmo(WeaponType, AmmoAmount);
		}
	}
	Destroy();
}

// Called every frame
void AAmmoPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

