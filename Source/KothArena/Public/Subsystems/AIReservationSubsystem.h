// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "AIReservationSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class KOTHARENA_API UAIReservationSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

// Public interface
public:
	bool SpotCanBeReserved(const FVector&, const float) const;
	bool ReserveSpot(const AActor*, const FVector&, const float Tolerance);
	void ReleaseSpot(const AActor*);

// Properties
protected:
	UPROPERTY()
	TMap<const AActor*, FVector> Reservations;
};
