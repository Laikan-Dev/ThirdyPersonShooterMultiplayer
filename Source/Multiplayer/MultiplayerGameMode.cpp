// Copyright Epic Games, Inc. All Rights Reserved.

#include "MultiplayerGameMode.h"
#include "MultiplayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "MultiplayerPlayerState.h"
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

void AMultiplayerGameMode::ChooseTeam_Implementation(AMultiplayerCharacter* CurrentPlayer, ETeam ChosenTeam)
{
	AMultiplayerCharacter* Player = Cast<AMultiplayerCharacter>(CurrentPlayer);
	if (Player)
	{
		
		AMultiplayerPlayerState* PlayerState = Cast<AMultiplayerPlayerState>(Player->GetPlayerState());
		if (PlayerState)
		{
			PlayerState->PlayerTeam = ChosenTeam;

			if (GEngine)
			{
				switch (ChosenTeam)
				{
				case ETeam::ET_RedTeam:
				{
					FString ChosenTeamMessage = FString::Printf(TEXT("RedTeam"));
					GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Green, ChosenTeamMessage);
					Player->GetMesh()->SetMaterial(0, Player->Red);
				}
				break;
				case ETeam::ET_BlueTeam:
				{
					FString ChosenTeamMessage = FString::Printf(TEXT("BlueTeam"));
					GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Green, ChosenTeamMessage);
					Player->GetMesh()->SetMaterial(0, Player->Blue);
				}
				break;
				case ETeam::ET_NoTeam:
					break;
				default:
					break;
				}

			}
		}
	}
}
