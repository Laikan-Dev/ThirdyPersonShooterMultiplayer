// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileRocket.h"
#include "Kismet/GameplayStatics.h"
#include  "NiagaraFunctionLibrary.h"
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

	RocketMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("RocketMesh"));
	RocketMesh->SetupAttachment(RootComponent);
	RocketMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

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
	if (TrailSystem)
	{
		TrailSystemComp = UNiagaraFunctionLibrary::SpawnSystemAttached(TrailSystem, GetRootComponent(), FName(), GetActorLocation(), GetActorRotation(),
			EAttachLocation::Type::KeepWorldPosition,  false);
	}
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
	if (OtherActor == GetOwner()) return;
	APawn* FiringPawn = GetInstigator();
	if (FiringPawn && HasAuthority())
	{
		AController* FiringController = FiringPawn->GetController();
		if (FiringController)
		{
			UGameplayStatics::ApplyRadialDamageWithFalloff(this, Damage, 10.f, GetActorLocation(),
				200.f, 500.f, 1.f, UDamageType::StaticClass(),
				TArray<AActor*>(), this, FiringController);
			GetWorldTimerManager().SetTimer(DestroyTimer, this, &AProjectileRocket::DestroyTimeFinished, DestroyTime);

			if (ImpactParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, GetActorTransform());
			}
			if (HitSound)
			{
				UGameplayStatics::PlaySoundAtLocation(this, HitSound, GetActorLocation());
			}
			if (RocketMesh)
			{
				RocketMesh->SetVisibility(false);
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
	}
	
}

void AProjectileRocket::DestroyTimeFinished()
{
	Destroy();
}

// Called every frame
void AProjectileRocket::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

