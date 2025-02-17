#pragma once

UENUM(BlueprintType)
enum class EPlayerOverlayState : uint8
{
	EPS_Unarmed UMETA(DisplayName = "Unarmed"),
	EPS_Melee UMETA(DisplayName = "Melee"),
	EPS_Pistol UMETA(DisplayName = "Pistol"),
	EPS_Rifle UMETA(DisplayName = "Rifle"),
};