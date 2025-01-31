// Copyright Epic Games, Inc. All Rights Reserved.

#include "MultiplayerGameMode.h"
#include "MultiplayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "MultiplayerPlayerState.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "UObject/ConstructorHelpers.h"

AMultiplayerGameMode::AMultiplayerGameMode()
{
	PlayerStateClass = AMultiplayerPlayerState::StaticClass();
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void AMultiplayerGameMode::ScoreWidgetSpawn_Implementation()
{
	if (CaptureFlagWidget)
	{
		APlayerController* PlayerController = Cast<APlayerController>(PlayerControllerClass);
		UUserWidget* Score = CreateWidget<UUserWidget>(PlayerController, CaptureFlagWidget);
		if (Score)
		{
			Score->AddToViewport();

		}
	}
}

void AMultiplayerGameMode::BeginPlay()
{
	Super::BeginPlay();
	ScoreWidgetSpawn();

	
}
