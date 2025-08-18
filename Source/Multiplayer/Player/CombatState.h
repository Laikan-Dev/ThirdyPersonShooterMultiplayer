#pragma once

UENUM(BlueprintType)
enum class ECombatState : uint8
{
	ECS_Idle UMETA(DisplayName = "Idle"),
	ECS_Reloading UMETA(DisplayName = "Reloading"),

	ECS_MAX UMETA(DisplayName = "DefaultMAX")
};