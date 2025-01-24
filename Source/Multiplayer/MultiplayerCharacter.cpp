// Copyright Epic Games, Inc. All Rights Reserved.

#include "MultiplayerCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"
#include "MPProjectile.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"


DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AMultiplayerCharacter

AMultiplayerCharacter::AMultiplayerCharacter()
{
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
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void AMultiplayerCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	SelectTeam();
}

void AMultiplayerCharacter::OnRep_CurrentHealth()
{
	OnHealthUpdate();
}
void AMultiplayerCharacter::OnRep_OnDeath()
{
	OnDeathUpdate();
}
void AMultiplayerCharacter::OnDeathUpdate()
{
	if (bIsDead == true)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Emerald, FString(TEXT("Ragdol")));
		ActiveRagdol();
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Emerald, FString(TEXT("NotRagdol")));
	}
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

	//Replicate Current Health
	DOREPLIFETIME(AMultiplayerCharacter, CurrentHealth);
	DOREPLIFETIME(AMultiplayerCharacter, CurrentTeam);
	DOREPLIFETIME(AMultiplayerCharacter, bIsDead);
}

void AMultiplayerCharacter::OnHealthUpdate()
{
	if (IsLocallyControlled())
	{
		FString healthMessage = FString::Printf(TEXT("You now have %f Health remaining"), CurrentHealth);
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Blue, healthMessage);

		if (CurrentHealth <= 0)
		{
			FString deathMessage = FString::Printf(TEXT("You have been killed"));
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, deathMessage);
			bIsDead = true;
		}
	}
	if (GetLocalRole() == ROLE_Authority)
	{
		FString healthMessage = FString::Printf(TEXT("%s now has %f health remaining."), *GetFName().ToString(), CurrentHealth);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, healthMessage);
	}
}

void AMultiplayerCharacter::SetCurrentHealth(float healthValue)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		CurrentHealth = FMath::Clamp(healthValue, 0.f, MaxHealth);
		OnHealthUpdate();
	}
}

float AMultiplayerCharacter::TakeDamage(float DamageTaken, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float damageApplied = CurrentHealth - DamageTaken;
	SetCurrentHealth(damageApplied);
	return damageApplied;
}

void AMultiplayerCharacter::StartFire()
{
	if (!bIsFiringWeapon)
	{
		bIsFiringWeapon = true;
		UWorld* World = GetWorld();
		World->GetTimerManager().SetTimer(FiringTimer, this, &AMultiplayerCharacter::StopFire, FireRate, false);
		HandleFire();
		
	}
	

}

void AMultiplayerCharacter::StopFire()
{
	bIsFiringWeapon = false;
}
void AMultiplayerCharacter::ActiveRagdol_Implementation()
{
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetAllBodiesSimulatePhysics(true);
}

void AMultiplayerCharacter::ServerSetTeam_Implementation(ETeam NewTeam)
{
	CurrentTeam = NewTeam;
	OnRep_PlayerTeam();
}

void AMultiplayerCharacter::SelectTeam_Implementation()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
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

void AMultiplayerCharacter::HandleFire_Implementation()
{
	FVector spawnLocation = GetActorLocation() + (GetActorRotation().Vector() * 100.0f) + (GetActorUpVector() * 50.0f); 
	FRotator spawnRotation = GetActorRotation();
	
	FActorSpawnParameters spawnParameters;
	spawnParameters.Instigator = GetInstigator();
	spawnParameters.Owner = this;

	AMPProjectile* spawnProjectile = GetWorld()->SpawnActor<AMPProjectile>(spawnLocation, spawnRotation, spawnParameters);
}

//////////////////////////////////////////////////////////////////////////
// Input

void AMultiplayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMultiplayerCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMultiplayerCharacter::Look);

		//Firing Projectiles
		EnhancedInputComponent->BindAction(FireInput, ETriggerEvent::Started, this, &AMultiplayerCharacter::StartFire);

		EnhancedInputComponent->BindAction(RedInput, ETriggerEvent::Started, this, &AMultiplayerCharacter::ChoseRed);
		EnhancedInputComponent->BindAction(BlueInput, ETriggerEvent::Started, this, &AMultiplayerCharacter::ChoseBlue);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AMultiplayerCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

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
