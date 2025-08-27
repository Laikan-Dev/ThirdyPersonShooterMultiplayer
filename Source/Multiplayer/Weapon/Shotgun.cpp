// Fill out your copyright notice in the Description page of Project Settings.


#include "Shotgun.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Multiplayer/MultiplayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"


// Sets default values
AShotgun::AShotgun()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AShotgun::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AShotgun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AShotgun::Fire(const FVector& HitTarget)
{
	ABaseWeapon::Fire(HitTarget);
	
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr) return;
	AController* InstigatorController = OwnerPawn->GetController();

	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (MuzzleFlashSocket)
	{
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		FVector Start = SocketTransform.GetLocation();
		uint32 Hits = 0;
		TMap<AMultiplayerCharacter*, uint32> HitMap;
		TMap<AMultiplayerCharacter*, uint32> HeadShotHitMap;
		for ( uint32 i = 0; i < NumberOfPellets; i++)
		{
			FHitResult FireHit;
			WeaponTraceHit(Start, HitTarget, FireHit);

			AMultiplayerCharacter* MultiplayerCharacter = Cast<AMultiplayerCharacter>(FireHit.GetActor());
			if (MultiplayerCharacter && HasAuthority() && InstigatorController)
			{
				const bool bHeadShot = FireHit.BoneName.ToString() == FString("head");
				if (bHeadShot)
				{
					if (HeadShotHitMap.Contains(MultiplayerCharacter))HeadShotHitMap[MultiplayerCharacter]++;
					else HeadShotHitMap.Emplace(MultiplayerCharacter, 1);
				}
				else
				{
					if (HitMap.Contains(MultiplayerCharacter))HitMap[MultiplayerCharacter]++;
					else HitMap.Emplace(MultiplayerCharacter, 1);
				}

				
			}
			if (ImpactParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, FireHit.ImpactPoint, FireHit.ImpactNormal.Rotation());
			}
			if (HitSound)
			{
				UGameplayStatics::PlaySoundAtLocation(this, HitSound, FireHit.ImpactPoint, .5f,
					FMath::FRandRange(-.5f, .5f));
			}
		}
		TArray<AMultiplayerCharacter*> HitCharacters;

		//Character hit to total damage
		TMap<AMultiplayerCharacter*, float> DamageMap;

		// Calculate body shot damage by multiplying times hit x Damage
		for (auto HitPair : HitMap)
		{
			if (HitPair.Key && HasAuthority() && InstigatorController)
			{
				DamageMap.Emplace(HitPair.Key, HitPair.Value * Damage);
				HitCharacters.AddUnique(HitPair.Key);
			}
		}

		// Calculate head shot damage by multiplying times hit x HeadShotDamage - store in damagemap
		for (auto HeadShotHitPair : HeadShotHitMap)
		{
			if (HeadShotHitPair.Key && HasAuthority() && InstigatorController)
			{
				if (DamageMap.Contains(HeadShotHitPair.Key)) DamageMap[HeadShotHitPair.Key] += HeadShotHitPair.Value * GetHeadShotDamage();
				else DamageMap.Emplace(HeadShotHitPair.Key, HeadShotHitPair.Value * GetHeadShotDamage());
				HitCharacters.AddUnique(HeadShotHitPair.Key);
			}
		}
		// Loop through DamageMap to get total damage for each character
		for (auto DamagePair : DamageMap)
		{
			if (DamagePair.Key && InstigatorController)
			{
				bool bCauseAuthDamage = OwnerPawn->IsLocallyControlled();
				if (HasAuthority() && bCauseAuthDamage)
				{
					UGameplayStatics::ApplyDamage(DamagePair.Key, DamagePair.Value, InstigatorController,
						this, UDamageType::StaticClass());
				}
			}
		}
	}
}