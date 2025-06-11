// Fill out your copyright notice in the Description page of Project Settings.
#include "BaseWeapon.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Multiplayer/MultiplayerCharacter.h"
#include "WeaponInformation.h"
#include "Multiplayer/MultiplayerGameMode.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "Casing.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Multiplayer/Public/DataAsset/WeaponsDataAsset.h"

// Sets default values
ABaseWeapon::ABaseWeapon()
{
	bReplicates = true;
	SetReplicateMovement(true);
	SetReplicates(true);

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	RootComponent = SphereComponent;
	
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SkeletalMesh->SetupAttachment(RootComponent);
	if (WeaponData)
	{
		SkeletalMesh->SetSkeletalMesh(WeaponData->GetWeaponStats().Mesh);
	}
	SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);


	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	PickupWidget->SetupAttachment(RootComponent);

	//ShootingAnim = WeaponData->GetWeaponStats().FireAnimation;    //WeaponInformation.FireAnimation;
}

void ABaseWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABaseWeapon, WeaponState);
}

void ABaseWeapon::ShowPickupWidget(bool bShowWidget)
{
	if (PickupWidget)
	{
		PickupWidget->SetVisibility(bShowWidget);
	}
}

void ABaseWeapon::OnRep_WeaponState()
{
	switch (WeaponState)
	{
	case EWeaponState::EWS_Initial:
		break;
	case EWeaponState::EWS_Equipped:
	{
		ShowPickupWidget(false);
	}
		break;
	case EWeaponState::EWS_Dropped:
		break;
	case EWeaponState::EWS_MAX:
		break;
	default:
		break;
	}
}

// Called when the game starts or when spawned
void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority())
	{
		SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ABaseWeapon::OnComponentBeginOverlap);
		SphereComponent->OnComponentEndOverlap.AddDynamic(this, &ABaseWeapon::OnComponentEndOverlap);
	}
	if (PickupWidget)
	{
		PickupWidget->SetVisibility(false);
	}
	
}
void ABaseWeapon::PickUp_Implementation(AMultiplayerCharacter* Player)
{
	if (Player)
	{
		if (HasAuthority())
		{
			Player->MC_SetCurrentWeapon(WeaponData);
			Destroy();
		}
		else
		{
			Player->Server_SetCurrentWeapon(WeaponData);
			Destroy();
		}
	}
	
}

void ABaseWeapon::SetWeaponState(EWeaponState State)
{
	WeaponState = State;
	switch (WeaponState)
	{
	case EWeaponState::EWS_Initial:
		break;
	case EWeaponState::EWS_Equipped:
	{
		ShowPickupWidget(false);
		SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	break;
	case EWeaponState::EWS_Dropped:
		break;
	case EWeaponState::EWS_MAX:
		break;
	default:
		break;
	}
}

void ABaseWeapon::Fire_Implementation(const FVector& HitTarget)
{
	if (WeaponData->GetWeaponStats().FireAnimation)
	{
		SkeletalMesh->PlayAnimation(WeaponData->GetWeaponStats().FireAnimation, false);
	}
	if (WeaponData->GetWeaponStats().CasingClass)
	{
		const USkeletalMeshSocket* AmmoEjectSocket = SkeletalMesh->GetSocketByName(FName("AmmoEject"));
		if (AmmoEjectSocket)
		{
			FTransform SocketTransform = AmmoEjectSocket->GetSocketTransform(SkeletalMesh);
			
			UWorld* World = GetWorld();
			if (World)
			{
				//World->SpawnActor<ACasing>(WeaponInformation.CasingClass, SocketTransform.GetLocation(), SocketTransform.GetRotation().Rotator());
				
			}
		}
	}
}

void ABaseWeapon::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMultiplayerCharacter* Player = Cast<AMultiplayerCharacter>(OtherActor);
	if (Player)
	{
		Player->SetOverlappingWeapon(this);
	}
}

void ABaseWeapon::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AMultiplayerCharacter* Player = Cast<AMultiplayerCharacter>(OtherActor);
	if (Player)
	{
		Player->SetOverlappingWeapon(nullptr);
	}
}



