// Copyright Epic Games, Inc. All Rights Reserved.

#include "MultiplayerGameMode.h"
#include "MultiplayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

AMultiplayerGameMode::AMultiplayerGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void AMultiplayerGameMode::ChooseTeam_Implementation(AMultiplayerCharacter* CurrentPlayer, ETeam ChosenTeam)
{
	AMultiplayerCharacter* Player = Cast<AMultiplayerCharacter>(CurrentPlayer);
	if (Player)
	{
		Player->CurrentTeam = ChosenTeam;
		if (GEngine)
		{
			switch (ChosenTeam)
			{
			case ETeam::None:
				break;
			case ETeam::Red:
			{
				FString ChosenTeamMessage = FString::Printf(TEXT("RedTeam"));
				GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Green, ChosenTeamMessage);
			}
			break;
			case ETeam::Blue:
			{
				FString ChosenTeamMessage = FString::Printf(TEXT("BlueTeam"));
				GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Green, ChosenTeamMessage);
			}
			break;
			default:
				break;
			}

		}
	}
}
