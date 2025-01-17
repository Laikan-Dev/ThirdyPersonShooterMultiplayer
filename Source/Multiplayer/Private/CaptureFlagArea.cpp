// Fill out your copyright notice in the Description page of Project Settings.


#include "CaptureFlagArea.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"

// Sets default values
ACaptureFlagArea::ACaptureFlagArea()
{
	//InitialStats
	MaxCaptureTime = 100;
	CurrentCaptureTime = 0;

	bReplicates = true;
	TriggerCollision = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	TriggerCollision->OnComponentBeginOverlap.AddDynamic(this, &ACaptureFlagArea::OnOverlapCollision);
	TriggerCollision->OnComponentEndOverlap.AddDynamic(this, &ACaptureFlagArea::OnEndOverlapCollision);
	RootComponent = TriggerCollision;
	Flag = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Flag->SetupAttachment(RootComponent);
	


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
			FString captureMessage = FString::Printf(TEXT("RedTeam Capture the flag!"));
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Green, captureMessage);
		}
	}
}

void ACaptureFlagArea::SetCurrentCaptureTime(float CaptureTimeValue)
{
	CurrentCaptureTime = FMath::Clamp(CaptureTimeValue, 0.f, MaxCaptureTime);
	OnCaptureTimeUpdate();
}

// Called every frame
void ACaptureFlagArea::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACaptureFlagArea::OnOverlapCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	if (OtherActor)
	{
		float Capature = CurrentCaptureTime + 1 ;
		SetCurrentCaptureTime(Capature);
		if (GEngine)
		{
			FString CapturingMessage = FString::Printf(TEXT("Red Team Are capturing %f the flag"), Capature);
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue, CapturingMessage);
		}
	}
	

}

void ACaptureFlagArea::OnEndOverlapCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		float Capture = CurrentCaptureTime--;
		SetCurrentCaptureTime(Capture);
	}
}

