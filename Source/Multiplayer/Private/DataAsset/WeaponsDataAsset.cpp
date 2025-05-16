// Fill out your copyright notice in the Description page of Project Settings.


#include "DataAsset/WeaponsDataAsset.h"

FWeaponInformation UWeaponsDataAsset::GetWeaponStats()
{
	return WeaponStats;
}

FCrosshairInfo UWeaponsDataAsset::GetCrosshairInfo()
{
	return CrosshairInfo;
}
