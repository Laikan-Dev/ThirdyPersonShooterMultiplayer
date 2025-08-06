// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Team.h"
#include "Multiplayer/AbilitiesSystem/MultiplayerASComponent.h"

AMultiplayerPlayerState::AMultiplayerPlayerState()
{
	TimeLeft = 300;
	RedTeamScore = 0;
	BlueTeamScore = 0;

	SetNetUpdateFrequency(100.f);
	AbilitySystemComponent = CreateDefaultSubobject<UMultiplayerASComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
}
void AMultiplayerPlayerState::BeginPlay()
{
	Super::BeginPlay();
	FTimerHandle GameTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(GameTimerHandle, this, &AMultiplayerPlayerState::UpdateTimer, 1.0f, true);

}

void AMultiplayerPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMultiplayerPlayerState, TimeLeft);
	DOREPLIFETIME(AMultiplayerPlayerState, RedTeamScore);
	DOREPLIFETIME(AMultiplayerPlayerState, BlueTeamScore);
}

void AMultiplayerPlayerState::OnRep_TimeLeft()
{
}

void AMultiplayerPlayerState::OnRep_TeamScore()
{
}

void AMultiplayerPlayerState::SetTimeLeft(int32 NewTime)
{
	if (HasAuthority())
	{
		TimeLeft = NewTime;
		OnRep_TimeLeft();
	}
}

void AMultiplayerPlayerState::SetScore(ETeam Team, int32 NewScore)
{
	if (HasAuthority())
	{
		switch (Team)
		{
		case ETeam::ET_RedTeam:
		{
			RedTeamScore = NewScore;
		}
		break;
		case ETeam::ET_BlueTeam:
		{
			BlueTeamScore = NewScore;
		}
		break;
		case ETeam::ET_NoTeam:
			break;
		}
		OnRep_Score();
	}
}

void AMultiplayerPlayerState::UpdateTimer()
{
	SetTimeLeft(FMath::Max(TimeLeft - 1, 0));
	if (TimeLeft <= 0)
	{

	}
}

UAbilitySystemComponent* AMultiplayerPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
 