// Fill out your copyright notice in the Description page of Project Settings.


#include "Utils/PlayerUtils.h"

bool UPlayerUtils::ComputeScreenCenterAndDirection(const APlayerController* Player, FVector& CenterLocation,
	FVector& CenterDirection)
{
	if (Player == nullptr)
	{
		return false;
	}

	if (Player->PlayerCameraManager == nullptr)
	{
		FRotator ViewpointDirection;
		Player->GetActorEyesViewPoint(CenterLocation, ViewpointDirection);
		CenterDirection = ViewpointDirection.Vector();
	}
	else
	{
		CenterLocation = Player->PlayerCameraManager->GetCameraLocation();
		CenterDirection = Player->GetControlRotation().Vector();
	}

	return true;

}
