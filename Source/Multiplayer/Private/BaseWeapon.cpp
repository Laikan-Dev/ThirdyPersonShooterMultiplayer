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
#include "Components/SkeletalMeshComponent.h"
#include "Multiplayer/Public/DataAsset/WeaponsDataAsset.h"
#include "MultiplayerPlayerController.h"
#include "CombatComponent.h"
// Sets default values
ABaseWeapon::ABaseWeapon()
{
	bReplicates = true;
	AActor::SetReplicateMovement(true);

	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	RootComponent = SkeletalMesh;
	SkeletalMesh->MarkRenderStateDirty();
	SkeletalMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SkeletalMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_PURPLE);
	SkeletalMesh->MarkRenderStateDirty();
	EnableCustomDepth(true);


	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->SetupAttachment(RootComponent);

	if (WeaponData)
	{
		SkeletalMesh->SetSkeletalMesh(WeaponData->GetWeaponStats().Mesh);
	}

	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	PickupWidget->SetupAttachment(RootComponent);

	//ShootingAnim = WeaponData->GetWeaponStats().FireAnimation;    //WeaponInformation.FireAnimation;
}

void ABaseWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABaseWeapon, WeaponState);
	DOREPLIFETIME(ABaseWeapon, Ammo);
}

void ABaseWeapon::OnRep_Owner()
{
	Super::OnRep_Owner();
	if (Owner == nullptr)
	{
		OwnerCharacter = nullptr;
		OwnerController = nullptr;
	}
	else
	{
		SetHUDAmmo();
	}
}

void ABaseWeapon::ShowPickupWidget(bool bShowWidget)
{
	if (PickupWidget)
	{
		PickupWidget->SetVisibility(bShowWidget);
	}
}

void ABaseWeapon::OnRep_Ammo()
{
	OwnerCharacter = OwnerController == nullptr ? Cast<AMultiplayerCharacter>(GetOwner()) : OwnerCharacter;
	if (OwnerCharacter && OwnerCharacter->GetCombatSystem() && IsFull())
	{
		OwnerCharacter->GetCombatSystem()->JumpToShotgunEnd();
	}
	SetHUDAmmo();
}

void ABaseWeapon::SetHUDAmmo()
{
	OwnerCharacter = OwnerCharacter == nullptr ? Cast<AMultiplayerCharacter>(GetOwner()) : OwnerCharacter;
	if (OwnerCharacter)
	{
		OwnerController = OwnerController == nullptr ? Cast<AMultiplayerPlayerController>(OwnerCharacter->Controller) : OwnerController;
		if (OwnerController)
		{
			OwnerController->SetHUDWeaponAmmo(Ammo);
		}
	}
}

void ABaseWeapon::EnableCustomDepth(bool bEnable)
{
	if (SkeletalMesh)
	{
		SkeletalMesh->SetRenderCustomDepth(bEnable);
	}
}

void ABaseWeapon::SpendRound()
{
	Ammo = FMath::Clamp(Ammo - 1, 0, MagCapacity);
	SetHUDAmmo();
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
			SkeletalMesh->SetSimulatePhysics(false);
			SkeletalMesh->SetEnableGravity(false);
			SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			if (WeaponType == EWeaponType::EWT_SubmachinGun)
			{
				SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
				SkeletalMesh->SetEnableGravity(true);
				SkeletalMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			}
			EnableCustomDepth(false);
			
	}
		break;
	case EWeaponState::EWS_Dropped:
		SkeletalMesh->SetSimulatePhysics(true);
		SkeletalMesh->SetEnableGravity(true);
		SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		SkeletalMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
		SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
		SkeletalMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_BLUE);
		SkeletalMesh->MarkRenderStateDirty();
		EnableCustomDepth(true);
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
		SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		SphereComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
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
			SkeletalMesh->SetSimulatePhysics(false);
			SkeletalMesh->SetEnableGravity(false);
			SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			if (WeaponType == EWeaponType::EWT_SubmachinGun)
			{
				SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
				SkeletalMesh->SetEnableGravity(true);
				SkeletalMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			}
			EnableCustomDepth(false);
	}
	break;
	case EWeaponState::EWS_Dropped:
		if (HasAuthority())
		{
			SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
		SkeletalMesh->SetSimulatePhysics(true);
		SkeletalMesh->SetEnableGravity(true);
		SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		SkeletalMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
		SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
		SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
		SkeletalMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_PURPLE);
		SkeletalMesh->MarkRenderStateDirty();
		EnableCustomDepth(true);

		break;
	case EWeaponState::EWS_MAX:
		break;
	default:
		break;
	}
}

bool ABaseWeapon::IsEmpty()
{
	return Ammo <= 0;
}

bool ABaseWeapon::IsFull()
{
	return Ammo == MagCapacity;
}

void ABaseWeapon::Fire(const FVector& HitTarget)
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
	SpendRound();
}

void ABaseWeapon::Dropped()
{
	SetWeaponState(EWeaponState::EWS_Dropped);
	FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
	SkeletalMesh->DetachFromComponent(DetachRules);
	SetOwner(nullptr);
	OwnerCharacter = nullptr;
	OwnerController = nullptr;
}

void ABaseWeapon::AddAmmo(int32 AmmoToAdd)
{
	Ammo = FMath::Clamp(Ammo - AmmoToAdd, 0 , MagCapacity);
	SetHUDAmmo();
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



