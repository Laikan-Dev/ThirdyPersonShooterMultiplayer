// Fill out your copyright notice in the Description page of Project Settings.


#include "CaptureFlagArea.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"
#include "MultiplayerPlayerState.h"
#include "CaptureFlagGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Multiplayer/MultiplayerCharacter.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetComponent.h"

// Sets default values
ACaptureFlagArea::ACaptureFlagArea()
{
	//InitialStats
	MaxCaptureTime = 100;
	CurrentCaptureTime = 0;
	CurrentTeam = ETeam::ET_NoTeam;

	bReplicates = true;
	TriggerCollision = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	TriggerCollision->SetCollisionObjectType(ECollisionChannel::ECC_Vehicle);
	TriggerCollision->OnComponentBeginOverlap.AddDynamic(this, &ACaptureFlagArea::OnOverlapCollision);
	TriggerCollision->OnComponentEndOverlap.AddDynamic(this, &ACaptureFlagArea::OnEndOverlapCollision);
	RootComponent = TriggerCollision;
	Flag = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Flag->SetupAttachment(RootComponent);
	

	ProgressBar = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	ProgressBar->SetupAttachment(RootComponent);
	ProgressBar->SetIsReplicated(true);
	
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACaptureFlagArea::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACaptureFlagArea::OnRep_CurrentCaptureTime()
{
	OnCaptureTimeUpdate();
}

void ACaptureFlagArea::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//Replicate Current CaptureTime
	DOREPLIFETIME(ACaptureFlagArea, CurrentCaptureTime);
}

void ACaptureFlagArea::OnCaptureTimeUpdate()
{
	if (CurrentCaptureTime >= MaxCaptureTime)
	{
		if(GEngine)
		{
			switch (CurrentTeam)
			{
			case ETeam::ET_RedTeam:
			{
				FString captureMessage = FString::Printf(TEXT("Red Team Capture the flag!"));
				GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Green, captureMessage);
				if (RedTeamColor)
				{
					Flag->SetMaterial(0, RedTeamColor);
					AddTeamScore();

				}
			}
				break;
			case ETeam::ET_BlueTeam:
			{
				FString captureMessage = FString::Printf(TEXT("Blue Team Capture the flag!"));
				GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Green, captureMessage);
				if (BlueTeamColor)
				{
					Flag->SetMaterial(0, BlueTeamColor);
					AddTeamScore();

				}
			}
				break;
			case ETeam::ET_NoTeam:
				break;
			default:
				break;
			}
			
			bCaptured = true;
		}
	}
}

void ACaptureFlagArea::SetCurrentCaptureTime_Implementation(float CaptureTimeValue)
{
	CurrentCaptureTime = FMath::Clamp(CaptureTimeValue, 0.f, MaxCaptureTime);
	OnCaptureTimeUpdate();
	ACaptureFlagGameState* GameState = Cast<ACaptureFlagGameState>(UGameplayStatics::GetGameState(GetWorld()));
	if (GameState)
	{
		AddProgressToWidget(CaptureTimeValue);
	}

	if (GEngine)
	{
		if (CaptureTimeValue > 0)
		{
			switch (CurrentTeam)
			{
			case ETeam::ET_RedTeam:
			{
				FString CapturingMessage = FString::Printf(TEXT("Red Team Are capturing %f the flag"), CurrentCaptureTime);
				GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, CapturingMessage);

			}
			break;
			case ETeam::ET_BlueTeam:
			{
				FString CapturingMessage = FString::Printf(TEXT("Blue Team Are capturing %f the flag"), CurrentCaptureTime);
				GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, CapturingMessage);
			}
			break;
			case ETeam::ET_NoTeam:
				break;
			}
		}
		else
		{
			CurrentTeam = ETeam::ET_NoTeam;
		}
	}
}

void ACaptureFlagArea::AddTeamScore_Implementation()
{
	ACaptureFlagGameState* GameState = Cast<ACaptureFlagGameState>(UGameplayStatics::GetGameState(GetWorld()));
	if (GameState)
	{
		GameState->SetScore(CurrentTeam, 1);
	}
}

void ACaptureFlagArea::AddProgressToWidget_Implementation(float value)
{

}

void ACaptureFlagArea::ContestingFlagArea()
{
	ACaptureFlagGameState* GameState = Cast<ACaptureFlagGameState>(UGameplayStatics::GetGameState(GetWorld()));
	if (GameState)
	{
		if (bContesting)
		{
			GameState->bContesting = true;
		}
		else
		{
			GameState->bContesting = false;
		}
	}
}

bool ACaptureFlagArea::CanIncreseCapture(AMultiplayerCharacter* CurrentPlayer, ETeam PlayerTeam)
{
	if (RedTeamPlayersArray.Num() > 0 && !bContesting)
	{
		CurrentTeam = ETeam::ET_RedTeam;
		return true;
	}
	if (BlueTeamPlayersArray.Num() > 0 && !bContesting)
	{
		CurrentTeam = ETeam::ET_BlueTeam;
		return true;
	}
	if (CurrentTeam == ETeam::ET_NoTeam)
	{
		return false;
	}
	return false;
}

void ACaptureFlagArea::AddPlayerToTeamArray(AMultiplayerCharacter* CurrentPlayer, ETeam PlayerTeam)
{
	if (CurrentPlayer)
	{
		switch (PlayerTeam)
		{
		case ETeam::ET_RedTeam:
			{
				RedTeamPlayersArray.Add(CurrentPlayer);
				if (BlueTeamPlayersArray.Num() > 0)
				{
					bContesting = true;
					ContestingFlagArea();
				}
			
			}
			break;
		case ETeam::ET_BlueTeam:
			{
				BlueTeamPlayersArray.Add(CurrentPlayer);
				if (RedTeamPlayersArray.Num() > 0)
				{
					bContesting = true;
					ContestingFlagArea();
				}
			}
			break;
		}
	}
}

void ACaptureFlagArea::RemovePlayerToTeamArray(AMultiplayerCharacter* CurrentPlayer, ETeam PlayerTeam)
{
	if (CurrentPlayer)
	{
		switch (PlayerTeam)
		{
		case ETeam::ET_RedTeam:
		{
			RedTeamPlayersArray.Remove(CurrentPlayer);

			if (RedTeamPlayersArray.Num() <= 0 && CurrentTeam == ETeam::ET_RedTeam)
			{
				CurrentTeam = ETeam::ET_NoTeam;
			}
			if (RedTeamPlayersArray.Num() <= 0)
			{
				bContesting = false;
				ContestingFlagArea();
			}
		}
		break;
		case ETeam::ET_BlueTeam:
		{
			BlueTeamPlayersArray.Remove(CurrentPlayer);
			if (BlueTeamPlayersArray.Num() <= 0 && CurrentTeam == ETeam::ET_BlueTeam)
			{
				CurrentTeam = ETeam::ET_NoTeam;
			}
			if (BlueTeamPlayersArray.Num() <= 0)
			{
				bContesting = false;
				ContestingFlagArea();
			}
		}
		break;
		}
	}
}

// Called every frame
void ACaptureFlagArea::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!bCaptured)
	{
		if (bCapturing)
		{
			float Capture = CurrentCaptureTime + 0.1;
			SetCurrentCaptureTime(Capture);

		}
		else
		{	
			float Capture = CurrentCaptureTime - 0.1;
			SetCurrentCaptureTime(Capture);

		}
		if(bContesting)
		{
			FString ContestingMessage = FString::Printf(TEXT("Teams are contesting the flag"));
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue, ContestingMessage);
		}
	}
}

void ACaptureFlagArea::OnOverlapCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMultiplayerCharacter* Player = Cast<AMultiplayerCharacter>(OtherActor);
	if (Player != nullptr)
	{
		AddPlayerToTeamArray(Player, Player->CurrentTeam);
		if (CanIncreseCapture(Player, Player->CurrentTeam))
		{
			bCapturing = true;
		}
		else
		{
			bCapturing = false;
		}
	}
}

void ACaptureFlagArea::OnEndOverlapCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AMultiplayerCharacter* Player = Cast<AMultiplayerCharacter>(OtherActor);
	if (Player != nullptr)
	{
		RemovePlayerToTeamArray(Player, Player->CurrentTeam);
		if (CanIncreseCapture(Player, Player->CurrentTeam))
		{
			bCapturing = true;
		}
		else
		{
			bCapturing = false;
		}
	}
}

