// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/AIReservationSubsystem.h"

bool UAIReservationSubsystem::SpotCanBeReserved(const FVector& SpotQuerier, const float Tolerance) const
{
	for (const TPair<const AActor*, FVector> Reservation : Reservations)
	{
		const FVector ReservedSpot = Reservation.Value;
		const float Distance = FVector::Dist(ReservedSpot, SpotQuerier);
		if (Distance <= Tolerance)
		{
			return false;
		}
	}

	return true;
}

bool UAIReservationSubsystem::ReserveSpot(const AActor* ActorQuerier, const FVector& SpotLocation, const float Tolerance)
{
	if (!SpotCanBeReserved(SpotLocation, Tolerance))
	{
		return false;
	}

	if (Reservations.Contains(ActorQuerier))
	{
		Reservations[ActorQuerier] = SpotLocation;
	}
	else
	{
		Reservations.Add(ActorQuerier, SpotLocation);
	}

	return true;
}

void UAIReservationSubsystem::ReleaseSpot(const AActor* ActorQuerier)
{
	Reservations.Remove(ActorQuerier);
}
