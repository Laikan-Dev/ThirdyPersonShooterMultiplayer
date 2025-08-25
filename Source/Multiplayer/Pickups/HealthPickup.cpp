// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthPickup.h"
#include "../MultiplayerCharacter.h"
#include "Multiplayer/Components/BuffComponent.h"


// Sets default values
AHealthPickup::AHealthPickup()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

// Called when the game starts or when spawned
void AHealthPickup::BeginPlay()
{
	Super::BeginPlay();
	
}

void AHealthPickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	AMultiplayerCharacter* PlayerCharacter = Cast<AMultiplayerCharacter>(OtherActor);
	if (PlayerCharacter)
	{
		UBuffComponent* BuffComponent = PlayerCharacter->GetBuffComponent();
		if (BuffComponent)
		{
			BuffComponent->Heal(HealAmount, HealingTime);
		}
	}
	Destroy();
}

// Called every frame
void AHealthPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

