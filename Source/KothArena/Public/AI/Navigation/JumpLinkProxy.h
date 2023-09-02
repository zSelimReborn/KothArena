// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Navigation/NavLinkProxy.h"
#include "JumpLinkProxy.generated.h"

/**
 * 
 */
UCLASS()
class KOTHARENA_API AJumpLinkProxy : public ANavLinkProxy
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

// Callbacks
protected:
	UFUNCTION()
	void JumpToDestination(AActor* MovingActor, const FVector& DestinationPoint);

// Properties
protected:
	UPROPERTY(EditAnywhere)
	FVector JumpMultiplier = FVector::ZeroVector;
};
