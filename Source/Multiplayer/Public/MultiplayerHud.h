// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/HUD.h"
#include "MultiplayerHud.generated.h"


USTRUCT(BlueprintType)
struct FHUDPackage
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairsCenter;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairsLeft;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairsRight;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairsTop;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairsBottom;

	float CrosshairSpread;
	FLinearColor CrosshairsColor;
};
/**
 * 
 */
UCLASS()
class MULTIPLAYER_API AMultiplayerHud : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;

private:
	void DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D Spread, FLinearColor CrosshairColor);

	UPROPERTY(EditAnywhere)
	float CrosshairSpreadMax = 16.f;

public:
	FHUDPackage HUDPackage;
	FORCEINLINE void SetHUDPackage(const FHUDPackage& Package) { HUDPackage = Package; }
};
