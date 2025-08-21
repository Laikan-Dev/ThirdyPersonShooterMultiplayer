// Copyright Epic Games, Inc. All Rights Reserved.

#include "MultiplayerCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"
#include "GameFramework/Controller.h"
#include "MultiplayerPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "CaptureFlagArea.h"
#include "CombatComponent.h"
#include "Multiplayer.h"
#include "Multiplayer/Public/MultiplayerCharAnimInstance.h"
#include "Niagara/Public/NiagaraComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Multiplayer/Public/DataAsset/WeaponsDataAsset.h"
#include "AsTheCaosRemainsGameMode.h"
#include "Windows/AllowWindowsPlatformTypes.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Player/ChaosRemPlayerState.h"
#include "Multiplayer/Weapon/WeaponTypes.h"
DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AMultiplayerCharacter

AMultiplayerCharacter::AMultiplayerCharacter()
{
	//Replicate
	bReplicates = true;
	SetReplicateMovement(true);

	//Niagra VFX
	AbilityVFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("VisualFx"));
	AbilityVFX->SetupAttachment(GetMesh());
	AbilityVFX->SetIsReplicated(true);

	//PlayerInfoWidget
	PlayerInfo = CreateDefaultSubobject<UWidgetComponent>(TEXT("PlayerInfo"));

	//DissolveTimeline
	DissolveTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("DissolveTimelineComponent"));

	//Scene Capture to UI
	UICameraVisualize = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("UICamera"));
	UICameraVisualize->SetupAttachment(GetMesh());
	UICameraVisualize->SetRelativeLocation(FVector(0.f, 46.f, 128.f));
	UICameraVisualize->SetRelativeRotation(FRotator(0.f, 0.f, -90.f));

	//DashDefaults
	TimeDashCooldown = 3.0;
	DashForceXY = 700;
	DashForceZ = 150.0;
	
	//Initial Health
	MaxHealth = 100;
	CurrentHealth = MaxHealth;

	//Initialize projectile Class
	ProjectileClass = AMPProjectile::StaticClass();

	//Init FireRate
	FireRate = 0.25f;
	bIsFiringWeapon = false;
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(360.0f, 0.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = NormalVelocity;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 0.f, 850.f);

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom->SocketOffset.Set(75.0, 68.0, 10.0);

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	//Create a Weapon Socket
	WeaponSocket = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HandSocket"));
	WeaponSocket->SetupAttachment(GetMesh(), TEXT("hand_rSocket"));
	WeaponSocket->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponSocket->SetIsReplicated(true);

	//Create CombatComponent
	CombatSystem = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	CombatSystem->SetIsReplicated(true);

	//Capsule Comp
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	//Mesh
	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void AMultiplayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (CombatSystem)
	{
		CombatSystem->Character = this;
	}
	
}

void AMultiplayerCharacter::PlayFireMontage(bool bAiming)
{
	if (CombatSystem == nullptr || CombatSystem->EquippedWeapon == nullptr) return;
	
	UAnimInstance* AnimInstanceRef = GetMesh()->GetAnimInstance();
	if (AnimInstanceRef && ShootingMontage)
	{
		AnimInstance->Montage_Play(ShootingMontage);
		FName SectionName;
		SectionName = bAiming ? FName("RifleAim") : FName("RifleHip");
		AnimInstanceRef->Montage_JumpToSection(SectionName);
	}
}

void AMultiplayerCharacter::PlayHitReactMontage()
{
	if (CombatSystem == nullptr || CombatSystem->EquippedWeapon == nullptr) return;
	
	UAnimInstance* AnimInstanceRef = GetMesh()->GetAnimInstance();
	if (AnimInstanceRef && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		FName SectionName("FromFront");
		AnimInstanceRef->Montage_JumpToSection(SectionName);
	}
}

void AMultiplayerCharacter::PlayDeathMontage()
{
	UAnimInstance* AnimInstanceRef = GetMesh()->GetAnimInstance();
	if (AnimInstanceRef && DeathMontage)
	{
		AnimInstance->Montage_Play(DeathMontage);
		/**
		FName SectionName;
		SectionName = bAiming ? FName("RifleAim") : FName("RifleHip");
		AnimInstanceRef->Montage_JumpToSection(SectionName);
		**/
	}
}

void AMultiplayerCharacter::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, class AController* InstigatorController, class AActor* DamageCauser)
{
	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.0f, MaxHealth);
	PlayHitReactMontage();
	UpdateHUDHealth();

	if (CurrentHealth <= 0)
	{
		AAsTheCaosRemainsGameMode* GameMode = GetWorld()->GetAuthGameMode<AAsTheCaosRemainsGameMode>();
		if (GameMode)
		{
			MultiplayerPlayerController = MultiplayerPlayerController == nullptr ? Cast<AMultiplayerPlayerController>(Controller) : MultiplayerPlayerController;
			AMultiplayerPlayerController* AttackerController = Cast<AMultiplayerPlayerController>(InstigatorController);
			GameMode->PlayerEliminated(this, MultiplayerPlayerController, AttackerController);
		}
	}
}

void AMultiplayerCharacter::UpdateHUDHealth()
{
	MultiplayerPlayerController = MultiplayerPlayerController == nullptr ? Cast<AMultiplayerPlayerController>(Controller) : MultiplayerPlayerController;
	if (MultiplayerPlayerController)
	{
		MultiplayerPlayerController->SetHudHealth(CurrentHealth, MaxHealth);
	}
}

void AMultiplayerCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	UpdateHUDHealth();
	if (HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &AMultiplayerCharacter::ReceiveDamage);
	}
	SelectTeam();
	bUseControllerRotationYaw = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	
	GetCharacterMovement()->RotationRate = FRotator(0.f, 720.f, 0.f);
	AnimInstance = Cast<UMultiplayerCharAnimInstance>(GetMesh()->GetAnimInstance());
}

void AMultiplayerCharacter::Destroyed()
{
	Super::Destroyed();

	AAsTheCaosRemainsGameMode* ChaosRemGameMode = Cast<AAsTheCaosRemainsGameMode>(UGameplayStatics::GetGameMode(this));
	bool bMatchNotInProgress = ChaosRemGameMode && ChaosRemGameMode->GetMatchState() != MatchState::InProgress;
	if (CombatSystem && CombatSystem->EquippedWeapon && bMatchNotInProgress)
	{
		CombatSystem->EquippedWeapon->Destroy();
	}
}

void AMultiplayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	RotateInPlace(DeltaTime);
	HideCameraIfCharacterClose();
	PollInit();
}

void AMultiplayerCharacter::RotateInPlace(float DeltaTime)
{
	if (bDisableGameplay)
	{
		bUseControllerRotationYaw = false;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		return;
	}
	if (CombatSystem && CombatSystem->EquippedWeapon) GetCharacterMovement()->bOrientRotationToMovement = false;
	if (CombatSystem && CombatSystem->EquippedWeapon) bUseControllerRotationYaw = true;
	if (GetLocalRole() > ENetRole::ROLE_SimulatedProxy && IsLocallyControlled())
	{
		AimOffset(DeltaTime);
	}
	else
	{
		TimeSinceLastMovementReplication += DeltaTime;
		if (TimeSinceLastMovementReplication > 0.25f)
		{
			OnRep_ReplicatedMovement();
		}
		CalculateAO_Pitch();
	}
}

void AMultiplayerCharacter::HideCameraIfCharacterClose()
{
	if (!IsLocallyControlled()) return;
	if ((FollowCamera->GetComponentLocation() - GetActorLocation()).Size() < CameraThreshold)
	{
		GetMesh()->SetVisibility(false);
		if (CombatSystem && CombatSystem->EquippedWeapon && CombatSystem->EquippedWeapon->GetWeaponMesh())
		{
			CombatSystem->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = true;
		}
	}
	else
	{
		GetMesh()->SetVisibility(true);
		if (CombatSystem && CombatSystem->EquippedWeapon && CombatSystem->EquippedWeapon->GetWeaponMesh())
		{
			CombatSystem->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = false;
		}
	}
}

float AMultiplayerCharacter::CalculateSpeed()
{
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;
	return Velocity.Size();
}

void AMultiplayerCharacter::SetOverlappingWeapon(ABaseWeapon* Weapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(false);
	}
	OverlappingWeapon = Weapon;
	if (IsLocallyControlled())
	{
		if (OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickupWidget(true);
		}
	}
}

void AMultiplayerCharacter::OnRep_OverlappingWeapon(ABaseWeapon* LastWeapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(true);
	}
	if (LastWeapon)
	{
		LastWeapon->ShowPickupWidget(false);
	}
}

bool AMultiplayerCharacter::IWeaponEquipped()
{
	return (CombatSystem && CombatSystem->EquippedWeapon);
}

void AMultiplayerCharacter::OnRep_CurrentHealth()
{
	UpdateHUDHealth();
	PlayHitReactMontage();
}
void AMultiplayerCharacter::OnRep_PlayerTeam()
{
	if (CurrentTeam == ETeam::ET_RedTeam && Red)
	{
		GetMesh()->SetMaterial(0, Red);
	}
	else if (CurrentTeam == ETeam::ET_BlueTeam && Blue)
	{
		GetMesh()->SetMaterial(0, Blue);
	}
}
//////////////////////////////////////////////////////////////////////////
// Replicate Properties
void AMultiplayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);


	DOREPLIFETIME(AMultiplayerCharacter, CurrentHealth); 	//Replicate Current Health
	DOREPLIFETIME(AMultiplayerCharacter, CurrentTeam);	    //Replicate Current Team//Replicate boolean Death for ABP
	DOREPLIFETIME(AMultiplayerCharacter, CurrentState);
	DOREPLIFETIME(AMultiplayerCharacter, WeaponData);
	DOREPLIFETIME(AMultiplayerCharacter, bDisableGameplay);
	DOREPLIFETIME_CONDITION(AMultiplayerCharacter, OverlappingWeapon, COND_OwnerOnly);
}

void AMultiplayerCharacter::OnRep_CurrentWeapon()
{
	OnCurrentWeaponUpdate();
}

void AMultiplayerCharacter::OnCurrentWeaponUpdate()
{
}

void AMultiplayerCharacter::SetCurrentHealth(float healthValue)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		CurrentHealth = FMath::Clamp(healthValue, 0.f, MaxHealth);
	}
}

void AMultiplayerCharacter::SetCurrentWeapon(UWeaponsDataAsset* CurrentWeapon)
{
	if(HasAuthority())
	{
		MC_SetCurrentWeapon(CurrentWeapon);
	}
	else
	{
		Server_SetCurrentWeapon(CurrentWeapon);
	}
}

void AMultiplayerCharacter::MC_SetCurrentWeapon_Implementation(UWeaponsDataAsset* CurrentWeapon)
{
	WeaponData = CurrentWeapon;
	WeaponSocket->SetSkeletalMesh(WeaponData->GetWeaponStats().Mesh);	
}

void AMultiplayerCharacter::Server_SetCurrentWeapon_Implementation(UWeaponsDataAsset* CurrentWeapon)
{
	MC_SetCurrentWeapon(CurrentWeapon);
	//WeaponInfo = CurrentWeapon;
	//WeaponSocket->SetSkeletalMesh(WeaponInfo.Mesh);
	//CurrentWeaponClass = WeaponInfo.WeaponClass;
	//FireRate = WeaponInfo.FireRate;
	//WeaponShotAnim = WeaponInfo.FireAnimation;
	
	
}

void AMultiplayerCharacter::StartFire()
{
	if (bIsAiming())
	{
		FTransform MuzzleTranform = WeaponSocket->GetSocketTransform(TEXT("FireSocket"));
		MuzzleTranform.SetToRelativeTransform(WeaponSocket->GetRelativeTransform());

		if (!bIsFiringWeapon)
		{
			bIsFiringWeapon = true;
			UWorld* World = GetWorld();
			World->GetTimerManager().SetTimer(FiringTimer, this, &AMultiplayerCharacter::StopFire, WeaponData->GetWeaponStats().FireRate, false);
			//Server_HandleFire(WeaponInfo.FireAnimation, MuzzleTranform.GetLocation(), MuzzleTranform.GetRotation().Rotator());
		}
	}
}

void AMultiplayerCharacter::StopFire()
{
	bIsFiringWeapon = false;
}

void AMultiplayerCharacter::ServerSetTeam_Implementation(ETeam NewTeam)
{
	CurrentTeam = NewTeam;
	OnRep_PlayerTeam();
}
//AimOffset
void AMultiplayerCharacter::AimOffset(float DeltaTime)
{
	if (CombatSystem && CombatSystem->EquippedWeapon == nullptr) return;
	float Speed = CalculateSpeed();
	bool bIsInAir = GetCharacterMovement()->IsFalling();
	
	if (Speed == 0.f && !bIsInAir) // standing still, not jumping
	{
		bRotateRootBone = true;
		FRotator CurrentAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotation, StartAimRotation);
		AO_Yaw = DeltaAimRotation.Yaw;
		if (TurningInPlace == ETurningInPlace::ETIP_NotTurning)
		{
			InterpAO_Yaw = AO_Yaw;
		}
		bUseControllerRotationYaw = true;
		TurnInPlace(DeltaTime);
	}
	if (Speed > 0.f || bIsInAir) // running, or jumping
	{
		bRotateRootBone = false;
		StartAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		AO_Yaw = 0.f;
		bUseControllerRotationYaw = true;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	}
	CalculateAO_Pitch();
}

void AMultiplayerCharacter::CalculateAO_Pitch()
{
	AO_Pitch = GetBaseAimRotation().Pitch;
	if (AO_Pitch > 90.f && !IsLocallyControlled())
	{
		//Map pitch from [270, 360) to [-90, 0)
		FVector2D InRange(270.f, 360.f);
		FVector2D OutRange(-90.f, 0.f);
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);
	}
}

void AMultiplayerCharacter::TurnInPlace(float DeltaTime)
{
	if (AO_Yaw > 90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Right;
	}
	else if (AO_Yaw < -90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Left;
	}
	if (TurningInPlace != ETurningInPlace::ETIP_NotTurning)
	{
		InterpAO_Yaw = FMath::FInterpTo(InterpAO_Yaw, 0.f, DeltaTime, 4.f);
		AO_Yaw = InterpAO_Yaw;
		if (FMath::Abs(AO_Yaw) < 15.f)
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
			StartAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		}
	}
}

bool AMultiplayerCharacter::bIsAiming()
{
	return (CombatSystem && CombatSystem->bAiming);
}

void AMultiplayerCharacter::OnAbilityVFX(bool bNewActivate)
{
	AbilityVFX->Activate(bNewActivate);
}

ABaseWeapon* AMultiplayerCharacter::GetEquippedWeapon()
{
	if (CombatSystem == nullptr) return nullptr;
	return CombatSystem->EquippedWeapon;

}

FVector AMultiplayerCharacter::GetHitTarget() const
{
	if (CombatSystem == nullptr) return FVector();

	return CombatSystem->HitTarget;
}

ECombatState AMultiplayerCharacter::GetCombatState() const
{
	if (CombatSystem == nullptr) return ECombatState::ECS_MAX;
	return CombatSystem->CombatState;
}

void AMultiplayerCharacter::SelectTeam_Implementation()
{
	AMultiplayerPlayerController* PlayerController = Cast<AMultiplayerPlayerController>(GetController());
	if (SelectTeamWidget)
	{
		UUserWidget* WidgetInstance = CreateWidget<UUserWidget>(PlayerController, SelectTeamWidget);
		if (WidgetInstance)
		{
			WidgetInstance->AddToViewport();
			PlayerController->SetShowMouseCursor(true);
		}
	}
}

//bool AMultiplayerCharacter::ServerSetTeam_Validation(ETeam NewTeam)
//{
	//return NewTeam == ETeam::ET_RedTeam || NewTeam == ETeam::ET_BlueTeam;
//}

void AMultiplayerCharacter::Server_HandleFire_Implementation(UAnimMontage* FireAnim, FVector MuzzleVector, FRotator MuzzleRotation)
{
	WeaponSocket->GetAnimInstance()->Montage_Play(FireAnim);
	FActorSpawnParameters spawnParameters;
	spawnParameters.Instigator = GetInstigator();
	spawnParameters.Owner = this;
	AMPProjectile* spawnProjectile = GetWorld()->SpawnActor<AMPProjectile>(ProjectileClass, MuzzleVector, MuzzleRotation, spawnParameters);
}

//////////////////////////////////////////////////////////////////////////
// Input

void AMultiplayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Add Input Mapping Context
	if (AMultiplayerPlayerController* PlayerController = Cast<AMultiplayerPlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		//EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Started, this, &AMultiplayerCharacter::StartJump);
		//EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMultiplayerCharacter::Move);
		//Running
		EnhancedInputComponent->BindAction(RunningAction, ETriggerEvent::Triggered, this, &AMultiplayerCharacter::Running);
		EnhancedInputComponent->BindAction(RunningAction, ETriggerEvent::Completed, this, &AMultiplayerCharacter::StopRunning);
		//Crounch
		EnhancedInputComponent->BindAction(CrounchAction, ETriggerEvent::Started, this, &AMultiplayerCharacter::StartCrounch);
		//Jump
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AMultiplayerCharacter::StartJump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AMultiplayerCharacter::StopJumping);
		//Dash
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Started, this, &AMultiplayerCharacter::StartDash);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMultiplayerCharacter::Look);

		//Aiming
		EnhancedInputComponent->BindAction(AimingInput, ETriggerEvent::Started, this, &AMultiplayerCharacter::StartAiming);
		EnhancedInputComponent->BindAction(AimingInput, ETriggerEvent::Completed, this, &AMultiplayerCharacter::StopAiming);

		//Firing Projectiles
		//EnhancedInputComponent->BindAction(FireInput, ETriggerEvent::Started, this, &AMultiplayerCharacter::StartFire);

		//FiringInput
		EnhancedInputComponent->BindAction(FireInput, ETriggerEvent::Started, this, &AMultiplayerCharacter::FireButtonPressed);
		EnhancedInputComponent->BindAction(FireInput, ETriggerEvent::Completed, this, &AMultiplayerCharacter::FireButtonReleased);

		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &AMultiplayerCharacter::ReloadButtonPressed);
		
		//PickupItem
		EnhancedInputComponent->BindAction(PickUpInput, ETriggerEvent::Started, this, &AMultiplayerCharacter::EquipItem);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AMultiplayerCharacter::OnRep_ReplicatedMovement()
{
	Super::OnRep_ReplicatedMovement();
	SimProxiesTurn();
	TimeSinceLastMovementReplication = 0.f;
}

void AMultiplayerCharacter::Elim()
{
	if (CombatSystem && CombatSystem->EquippedWeapon)
	{
		CombatSystem->EquippedWeapon->Dropped();
	}
	MulticastElim();
	GetWorldTimerManager().SetTimer(ElimTimer, this, &AMultiplayerCharacter::ElimTimerFinished, ElimDelay);
}

void AMultiplayerCharacter::MulticastElim_Implementation()
{
	if (MultiplayerPlayerController)
	{
		MultiplayerPlayerController->SetHUDWeaponAmmo(0);
	}
	bElimmed = true;
	PlayDeathMontage();

	//Start dissolve Character Effect
	if (DissolveMaterialInstance1 && DissolveMaterialInstance2)
	{
		DynamicDissolveMaterialInstance1 = UMaterialInstanceDynamic::Create(DissolveMaterialInstance1, this);
		GetMesh()->SetMaterial(0, DynamicDissolveMaterialInstance1);
		DynamicDissolveMaterialInstance1->SetScalarParameterValue(TEXT("DissolveValue"), 0.55f);
		DynamicDissolveMaterialInstance1->SetScalarParameterValue(TEXT("GlowValue"), 200.f);

		DynamicDissolveMaterialInstance2 = UMaterialInstanceDynamic::Create(DissolveMaterialInstance2, this);
		GetMesh()->SetMaterial(1, DynamicDissolveMaterialInstance2);
		DynamicDissolveMaterialInstance2->SetScalarParameterValue(TEXT("DissolveValue"), 0.55f);
		DynamicDissolveMaterialInstance2->SetScalarParameterValue(TEXT("GlowValue"), 200.f);
	}
	StartDissolve();
	//Disable Movement
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->StopMovementImmediately();
	bDisableGameplay = true;
	GetCharacterMovement()->DisableMovement();

	if (CombatSystem)
	{
		CombatSystem->FireButtonPressed(false);
	}
	//Disable Collision
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	bool bHideSniperScope = IsLocallyControlled() && CombatSystem && CombatSystem->EquippedWeapon && CombatSystem->EquippedWeapon->GetWeaponType() == EWeaponType::EWT_AssaultRifle;
	if (bHideSniperScope)
	{
		ShowSniperScopeWidget(false);
	}
}

void AMultiplayerCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();
	if(bDisableGameplay) return;
	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AMultiplayerCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AMultiplayerCharacter::ReloadButtonPressed()
{
	if(bDisableGameplay) return;
	if (CombatSystem)
	{
		CombatSystem->Reload();
	}
}

void AMultiplayerCharacter::PlayReloadMontage()
{
	if (CombatSystem == nullptr || CombatSystem->EquippedWeapon == nullptr) return;
	
	UAnimInstance* AnimInstanceRef = GetMesh()->GetAnimInstance();
	if (AnimInstanceRef && ReloadMontage)
	{
		AnimInstance->Montage_Play(ReloadMontage);
		FName SectionName;
		switch (CombatSystem->EquippedWeapon->GetWeaponType())
		{
		case EWeaponType::EWT_AssaultRifle:
			SectionName = FName("Rifle");
			break;

		case EWeaponType::EWT_RocketLauncher:
			SectionName = FName("Rifle");
			break;
		case EWeaponType::EWT_GrenadeLauncher:
			SectionName = FName("Rifle");
			break;
		case EWeaponType::EWT_Pistol:
			SectionName = FName("Rifle");
			break;
		case EWeaponType::EWT_SubmachinGun:
			SectionName = FName("Rifle");
			break;
		case EWeaponType::EWT_Shotgun:
			SectionName = FName("Rifle");
			break;
		case EWeaponType::EWT_SniperRifle:
			SectionName = FName("Rifle");
			break;
		default: ;
		}
		AnimInstanceRef->Montage_JumpToSection(SectionName);
	}
}

void AMultiplayerCharacter::ServerSetRuning_Implementation(bool bIsRunning)
{
	if (bIsRunning)
	{
		GetCharacterMovement()->MaxWalkSpeed = RunningVelocity;
		CameraBoom->SocketOffset.Set(30.0, 68.0, 10.0);
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = NormalVelocity;
		CameraBoom->SocketOffset.Set(75.0, 68.0, 10.0);
	}
}

void AMultiplayerCharacter::ElimTimerFinished()
{
	AAsTheCaosRemainsGameMode* GameMode = GetWorld()->GetAuthGameMode<AAsTheCaosRemainsGameMode>();
	if (GameMode)
	{
		GameMode->RequestRespawn(this, Controller);
	}
}

void AMultiplayerCharacter::UpdateDissolveMaterial(float DissolveValue)
{
	if (DynamicDissolveMaterialInstance1 && DynamicDissolveMaterialInstance2)
	{
		DynamicDissolveMaterialInstance1->SetScalarParameterValue(TEXT("DissolveValue"), DissolveValue);
		DynamicDissolveMaterialInstance2->SetScalarParameterValue(TEXT("DissolveValue"), DissolveValue);
	}
}

void AMultiplayerCharacter::StartDissolve()
{
	DissolveTrack.BindDynamic(this, &AMultiplayerCharacter::UpdateDissolveMaterial);
	if (DissolveCurve && DissolveTimeline)
	{
		DissolveTimeline->AddInterpFloat(DissolveCurve, DissolveTrack);
		DissolveTimeline->Play();
	}
}

void AMultiplayerCharacter::OnRep_Aiming()
{
	if (bIsAiming())
	{
		CurrentState = EPlayerOverlayState::EPS_Rifle;
		CameraBoom->SocketOffset.Set(250.0, 73.0, 60.0);
		GetCharacterMovement()->MaxWalkSpeed = AimingVelocity;
	}
	else
	{
		CurrentState = EPlayerOverlayState::EPS_Unarmed;
		CameraBoom->SocketOffset.Set(75.0, 68.0, 10.0);
		GetCharacterMovement()->MaxWalkSpeed = NormalVelocity;
	}
}

void AMultiplayerCharacter::StartAiming()
{
	if(bDisableGameplay) return;
	if (CombatSystem && CombatSystem->EquippedWeapon)
	{
		CombatSystem->SetAiming(true);
	}
}

void AMultiplayerCharacter::StopAiming()
{
	if (CombatSystem && CombatSystem->EquippedWeapon)
	{
		CombatSystem->SetAiming(false);
	}
}

void AMultiplayerCharacter::FireButtonPressed()
{
	if(bDisableGameplay) return;
	if (CombatSystem)
	{
		CombatSystem->FireButtonPressed(true);
	}
}

void AMultiplayerCharacter::FireButtonReleased()
{
	if(bDisableGameplay) return;
	if (CombatSystem)
	{
		CombatSystem->FireButtonPressed(false);
		CombatSystem->FireTimerFinished();
	}
}

void AMultiplayerCharacter::SimProxiesTurn()
{
	if (CombatSystem == nullptr || CombatSystem->EquippedWeapon == nullptr) return;
	bRotateRootBone = false;
	float Speed = CalculateSpeed();
	if (Speed > 0.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		return;
	}
	
	ProxyRotationLastFrame = ProxyRotation;
	ProxyRotation = GetActorRotation();
	ProxyRotationYaw = UKismetMathLibrary::NormalizedDeltaRotator(ProxyRotation, ProxyRotationLastFrame).Yaw;

	UE_LOG(LogTemp, Warning, TEXT("ProxyYaw: %f"), ProxyRotationYaw);
	if (FMath::Abs(ProxyRotationYaw) > TurnThreshold)
	{
		if (ProxyRotationYaw > TurnThreshold)
		{
			TurningInPlace = ETurningInPlace::ETIP_Right;
		}
		else if (ProxyRotationYaw < -TurnThreshold)
		{
			TurningInPlace = ETurningInPlace::ETIP_Left;
		}
		else
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		}
		return;
	}
	TurningInPlace = ETurningInPlace::ETIP_NotTurning;
}



void AMultiplayerCharacter::StartJump()
{
	if (bDisableGameplay) return;
	if (!bIsAiming())
	{
		Jump();
	}
}

void AMultiplayerCharacter::PollInit()
{
	if (PossessedPlayerState == nullptr)
	{
		PossessedPlayerState = GetPlayerState<AChaosRemPlayerState>();
		if (PossessedPlayerState)
		{
			PossessedPlayerState->AddToScore(0.f);
			PossessedPlayerState->AddToDefeats(0);
		}
	}
}

void AMultiplayerCharacter::ChoseRed()
{
	if (CurrentTeam == ETeam::ET_NoTeam)
	{
		if (HasAuthority())
		{
			ServerSetTeam(ETeam::ET_RedTeam);
		}
		else
		{
			ServerSetTeam(ETeam::ET_RedTeam);
		}
	}
}

void AMultiplayerCharacter::ChoseBlue()
{
	if (CurrentTeam == ETeam::ET_NoTeam)
	{
		if (HasAuthority())
		{
			ServerSetTeam(ETeam::ET_BlueTeam);
		}
		else
		{
			ServerSetTeam(ETeam::ET_BlueTeam);
		}
	}   
}

void AMultiplayerCharacter::Running()
{
	if (!bIsAiming())
	{
		GetCharacterMovement()->MaxWalkSpeed = RunningVelocity;
		CameraBoom->SocketOffset.Set(30.0, 68.0, 10.0);
		ServerSetRuning(true);
	}
}

void AMultiplayerCharacter::StopRunning()
{
	GetCharacterMovement()->MaxWalkSpeed = NormalVelocity;
	if (!bIsAiming())
	{
		CameraBoom->SocketOffset.Set(75.0, 68.0, 10.0);
	}
	ServerSetRuning(false);
}

void AMultiplayerCharacter::StartCrounch()
{
	if(bDisableGameplay) return;
	if (bIsCrouched)
	{
		UnCrouch();
		if (!bIsAiming())
		{
			CameraBoom->SocketOffset.Set(75.0, 68.0, 10.0);
		}
	}
	else
	{
		Crouch();
		if (!bIsAiming())
		{
			CameraBoom->SocketOffset.Set(100.f, 68.0, 10.0);
		}
	}
}

//DashFunctions

void AMultiplayerCharacter::StartDash()
{
	FTimerHandle TimerHandle;
	if (CanDash())
	{
		bIsDashing = true;
		AbilityDash(AnimInstance->MoveDirection);
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AMultiplayerCharacter::DashCooldown, TimeDashCooldown);
	}
}

void AMultiplayerCharacter::AddDashImpulse(ECharMovDirection Direction)
{
	
	FVector LelImpulse;
	switch (Direction)
	{
	case ECharMovDirection::None:
		{
		LelImpulse = GetActorForwardVector() * DashForceXY;
		}
		break;
	case ECharMovDirection::Forward:
	{
		LelImpulse = GetActorForwardVector() * DashForceXY;
	}
		break;
	case ECharMovDirection::Backward:
		{
		int32 Value = DashForceXY * -1;
		LelImpulse = GetActorForwardVector() * Value;
		}
		break;
	case ECharMovDirection::Left:
		{
		int32 Value = DashForceXY * -1;
		LelImpulse = GetActorRightVector() * Value;
		}
		break;
	case ECharMovDirection::Right:
		{
		LelImpulse = GetActorRightVector() * DashForceXY;
		}
		break;
	default:
		break;
	}
	LelImpulse.Z = LelImpulse.Z + DashForceZ;
	GetCharacterMovement()->AddImpulse(LelImpulse, true);
	return;
}

void AMultiplayerCharacter::DashCooldown()
{
	bIsDashing = false;
}

bool AMultiplayerCharacter::CanDash()
{
	if (!bIsDashing)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void AMultiplayerCharacter::AbilityDash(ECharMovDirection Direction)
{
	if (HasAuthority())
	{
		MC_AbilityDash(Direction);
	}
	else
	{
		Server_AbilityDash(Direction);
	}
}

void AMultiplayerCharacter::MC_AbilityDash_Implementation(ECharMovDirection Direction)
{
	LastDirection = Direction;
	OnAbilityVFX(true);
	AnimInstance->PlayDashMontage(LastDirection);
}

void AMultiplayerCharacter::Server_AbilityDash_Implementation(ECharMovDirection Direction)
{
	MC_AbilityDash(Direction);
}

void AMultiplayerCharacter::EquipItem()
{
	if(bDisableGameplay) return;
	if (CombatSystem)
	{
		if (HasAuthority())
		{
			CombatSystem->EquipWeapon(OverlappingWeapon);
		}
		else
		{
			ServerEquipItem();
		}
		
	}
}
void AMultiplayerCharacter::ServerEquipItem_Implementation()
{
	if (CombatSystem)
	{
		CombatSystem->EquipWeapon(OverlappingWeapon);
	}
}

