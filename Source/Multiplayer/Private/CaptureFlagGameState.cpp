// Fill out your copyright notice in the Description page of Project Settings.


#include "CaptureFlagGameState.h"
#include "Net/UnrealNetwork.h"
#include "Team.h"
#include "MultiplayerPlayerState.h"
#include "MultiplayerPlayerController.h"

ACaptureFlagGameState::ACaptureFlagGameState()
{
	TimeLeft = 300;
	RedTeamScore = 0;
	BlueTeamScore = 0;
}

void ACaptureFlagGameState::BeginPlay()
{
	Super::BeginPlay();
	FTimerHandle GameTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(GameTimerHandle, this, &ACaptureFlagGameState::UpdateTimer, 1.0f, true);

}

void ACaptureFlagGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACaptureFlagGameState, TimeLeft);
	DOREPLIFETIME(ACaptureFlagGameState, RedTeamScore);
	DOREPLIFETIME(ACaptureFlagGameState, BlueTeamScore);

}

TArray<AMultiplayerPlayerController*> ACaptureFlagGameState::GetAllPlayerController()
{
	TArray<AMultiplayerPlayerController*> PlayerControllers;
	for (APlayerState* PlayerState : PlayerArray)
	{
		if (PlayerState)
		{
			AMultiplayerPlayerController* PlayerController = Cast<AMultiplayerPlayerController>(PlayerState->GetOwner());
			if (PlayerController)
			{
				PlayerControllers.Add(PlayerController);
			}
		}
	}
	return PlayerControllers;
}

void ACaptureFlagGameState::OnRep_TimeLeft()
{
	if (TimeLeft <= 0)
	{
		OnTimeFinished();
	}
}

void ACaptureFlagGameState::OnRep_TeamScore()
{
}

void ACaptureFlagGameState::SetTimeLeft(int32 NewTime)
{
	if (HasAuthority())
	{
		TimeLeft = NewTime;
		OnRep_TimeLeft();
	}
}

void ACaptureFlagGameState::SetScore(ETeam Team, int32 NewScore)
{
	if (HasAuthority())
	{
		switch (Team)
		{
		case ETeam::ET_RedTeam:
		{
			RedTeamScore = RedTeamScore + NewScore;
		}
		break;
		case ETeam::ET_BlueTeam:
		{
			BlueTeamScore = BlueTeamScore + NewScore;
		}
		break;
		case ETeam::ET_NoTeam:
			break;
		}
		OnRep_TeamScore();
	}
}

void ACaptureFlagGameState::UpdateTimer()
{
	if (!bContesting)
	{
		SetTimeLeft(FMath::Max(TimeLeft - 1, 0));
	}
}

void ACaptureFlagGameState::OnTimeFinished()
{
	for (AMultiplayerPlayerController* PC : GetAllPlayerController())
	{
		if (PC)
		{
			if (RedTeamScore > BlueTeamScore)
			{
				PC->AddMatchResultWidget(ETeam::ET_RedTeam);
			}
			else
			{
				PC->AddMatchResultWidget(ETeam::ET_BlueTeam);
			}
		}
	}
}
