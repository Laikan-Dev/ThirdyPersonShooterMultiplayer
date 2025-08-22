// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileRocket.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "Sound/SoundCue.h"
#include "NiagaraComponent.h"
#include "NiagaraSystemInstanceController.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include "RocketMovementComponent.h"


// Sets default values
AProjectileRocket::AProjectileRocket()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("RocketMesh"));
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	RocketMovementComponent = CreateDefaultSubobject<URocketMovementComponent>(TEXT("RocketMovementComponent"));
	RocketMovementComponent->bRotationFollowsVelocity = true;
	RocketMovementComponent->SetIsReplicated(true); 
	
}

void AProjectileRocket::Destroyed()
{
}

// Called when the game starts or when spawned
void AProjectileRocket::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority())
	{
		BoxComponent->OnComponentHit.AddDynamic(this, &AProjectileRocket::OnProjectileImpact);
	}
	SpawnTrailSystem();
	if (ProjectileLoop && LoopingSoundAttenuation)
	{
		ProjectileLoopComp = UGameplayStatics::SpawnSoundAttached(ProjectileLoop, GetRootComponent(), FName(), GetActorLocation(),
			EAttachLocation::Type::KeepWorldPosition,  false, 1.f, 1.f, 0.f,
			LoopingSoundAttenuation, (USoundConcurrency*)nullptr, false);
	}
}

void AProjectileRocket::OnProjectileImpact(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	ExplodeDamage();
	StartDestroyTimer();
	if (ImpactParticles)
    	{
           	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, GetActorTransform()); 			
    	}
	if (HitSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, HitSound, GetActorLocation());
		}
	if (ProjectileMesh)
		{
			ProjectileMesh->SetVisibility(false);
		}
	if (BoxComponent)
		{
			BoxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	if (TrailSystemComp && TrailSystemComp->GetSystemInstanceController())
		{
			TrailSystemComp->GetSystemInstanceController()->Deactivate();
		}
	if (ProjectileLoopComp && ProjectileLoopComp->IsPlaying())
		{
			ProjectileLoopComp->Stop();
		}				
}

// Called every frame
void AProjectileRocket::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

