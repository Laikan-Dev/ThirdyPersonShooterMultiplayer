// Copyright Epic Games, Inc. All Rights Reserved.

#include "MultiplayerGameMode.h"
#include "MultiplayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "MultiplayerPlayerState.h"
#include "Multiplayer/Public/MultiplayerPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "UObject/ConstructorHelpers.h"
#include "Net/UnrealNetwork.h"

AMultiplayerGameMode::AMultiplayerGameMode()
{
	PlayerControllerClass = AMultiplayerPlayerController::StaticClass();
	PlayerStateClass = AMultiplayerPlayerState::StaticClass();
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void AMultiplayerGameMode::BeginPlay()
{
	Super::BeginPlay();
}

