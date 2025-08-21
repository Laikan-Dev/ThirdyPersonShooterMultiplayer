// Fill out your copyright notice in the Description page of Project Settings.


#include "MPProjectile.h"
#include "Components/BoxComponent.h"
#include "GameFramework/DamageType.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Multiplayer/MultiplayerCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Sound/SoundCue.h"
#include "Multiplayer/Multiplayer.h"


// Sets default values
AMPProjectile::AMPProjectile()
{
	bReplicates = true;
	SetReplicateMovement(true);

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("RootComponent"));
	BoxComponent->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	BoxComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	BoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BoxComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	BoxComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	BoxComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	BoxComponent->SetCollisionResponseToChannel(ECC_SkeletalMesh, ECollisionResponse::ECR_Block);

	RootComponent = BoxComponent;

	if (GetLocalRole() == ROLE_Authority)
	{
		BoxComponent->OnComponentHit.AddDynamic(this, &AMPProjectile::OnProjectileImpact);
	}

	//Definition for the ProjectileMovement Component.
	/**
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovementComponent->SetUpdatedComponent(BoxComponent);
	ProjectileMovementComponent->InitialSpeed = 15000.0f;
	ProjectileMovementComponent->MaxSpeed = 15000.0f;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
	**/

	DamageType = UDamageType::StaticClass();
	Damage = 10.0f;
}

// Called when the game starts or when spawned
void AMPProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (Tracer)
	{
		TracerComponent = UGameplayStatics::SpawnEmitterAttached(Tracer, BoxComponent, FName(), GetActorLocation(), GetActorRotation(), EAttachLocation::KeepWorldPosition);
	}
	
}

void AMPProjectile::Destroyed()
{
	Super::Destroyed();

	if (ImpactParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, GetActorTransform());
	}
	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, GetActorLocation());
	}
}

void AMPProjectile::OnProjectileImpact(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	/**
	AMultiplayerCharacter* Player = Cast<AMultiplayerCharacter>(OtherActor);
	if (Player)
	{
		if(TeamCheck(Player, Player->CurrentTeam))
		{
			UGameplayStatics::ApplyPointDamage(OtherActor, Damage, NormalImpulse, Hit, GetInstigator()->Controller, this, DamageType);
		}
	}
	**/
	Destroy();
}

bool AMPProjectile::TeamCheck(AMultiplayerCharacter* TargetPlayer, ETeam TargetTeam)
{
	if (TargetPlayer)
	{
		if (TargetPlayer->CurrentTeam == TargetTeam)
		{
			AMultiplayerCharacter* OwningPlayer = Cast<AMultiplayerCharacter>(GetOwner());
			if (OwningPlayer)
			{
				if (OwningPlayer->CurrentTeam == TargetTeam)
				{
					return false;
				}
				else
				{
					return true;
				}
			}
		}
	}
	return false;
}


