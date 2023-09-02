// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Navigation/JumpLinkProxy.h"

#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

void AJumpLinkProxy::BeginPlay()
{
	Super::BeginPlay();

	OnSmartLinkReached.AddDynamic(this, &AJumpLinkProxy::JumpToDestination);
}

void AJumpLinkProxy::JumpToDestination(AActor* MovingActor, const FVector& DestinationPoint)
{
	if (MovingActor != nullptr)
	{
		ACharacter* Character = Cast<ACharacter>(MovingActor);
		if (Character != nullptr)
		{
			const FVector StartingPoint = Character->GetActorLocation();
			FVector JumpVelocity = FVector::ZeroVector;
			if (UGameplayStatics::SuggestProjectileVelocity_CustomArc(GetWorld(), JumpVelocity, StartingPoint, DestinationPoint))
			{
				JumpVelocity *= JumpMultiplier;
				Character->LaunchCharacter(JumpVelocity, true, true);
				return;
			}
		}
	}

	ResumePathFollowing(MovingActor);
}
