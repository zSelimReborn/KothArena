// Fill out your copyright notice in the Description page of Project Settings.


#include "Utils/PlayerUtils.h"

#include "Characters/BaseCharacter.h"
#include "Kismet/GameplayStatics.h"

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

void UPlayerUtils::RotateToTarget(AActor* ActorToRotate, const AActor* Target)
{
	if (ActorToRotate == nullptr || Target == nullptr)
	{
		return;
	}

	const FVector DirectionToTarget = Target->GetActorLocation() - ActorToRotate->GetActorLocation();
	const FRotator RotationToTarget = DirectionToTarget.Rotation();

	ActorToRotate->SetActorRotation(RotationToTarget);
}

TArray<AActor*> UPlayerUtils::FindMembersOfTeam(const UObject* WorldContextObject, const int32 TeamId)
{
	TArray<AActor*> MembersOfTeam;
	TArray<AActor*> Characters;

	// TODO - Will be optimized using a subsystem for spawning enemies
	UGameplayStatics::GetAllActorsOfClass(WorldContextObject, ABaseCharacter::StaticClass(), Characters);

	for (AActor* Actor : Characters)
	{
		ABaseCharacter* Character = Cast<ABaseCharacter>(Actor);
		if (Character != nullptr)
		{
			if (Character->GetTeamId() == TeamId)
			{
				MembersOfTeam.Add(Character);
			}
		}
	}

	return MembersOfTeam;
}
