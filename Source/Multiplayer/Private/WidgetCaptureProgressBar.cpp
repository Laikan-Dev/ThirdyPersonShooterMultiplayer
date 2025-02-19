// Fill out your copyright notice in the Description page of Project Settings.


#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/ProgressBar.h"
#include "WidgetCaptureProgressBar.h"
#include "Animation/WidgetAnimation.h"

void UWidgetCaptureProgressBar::UpdateBar(float CurrentValue, float MaxValue, ETeam Team)
{

	if (ProgressBar)
	{
		float Value = CurrentValue / MaxValue;

		ProgressBar->SetPercent(Value);

		switch (Team)
		{
		case ETeam::ET_RedTeam:
		{
			ProgressBar->SetFillColorAndOpacity(FLinearColor::Red);
		}
		break;
		case ETeam::ET_BlueTeam:
		{
			ProgressBar->SetFillColorAndOpacity(FLinearColor::Blue);
		}
		break;
		case ETeam::ET_NoTeam:
			break;
		default:
			break;
		}
	}
}

void UWidgetCaptureProgressBar::Contesting(bool bIsContesting)
{
	if (bIsContesting && ContestingAnimationIcon)
	{
		
		UUserWidget::PlayAnimation(ContestingAnimationIcon, 0);
	}
	else
	{
		UUserWidget::StopAnimation(ContestingAnimationIcon);
	}
}
