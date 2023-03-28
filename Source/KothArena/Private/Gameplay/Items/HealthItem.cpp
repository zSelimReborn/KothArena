// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Items/HealthItem.h"

#include "Characters/ShooterCharacter.h"

int32 AHealthItem::GetHealthAmountToRegen() const
{
	return FMath::RandRange(MinHealthRegen, MaxHealthRegen);
}

bool AHealthItem::ConsumeItem(AActor* InstigatorActor)
{
	AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(InstigatorActor);
	if (ShooterCharacter)
	{
		const int32 HealthAmount = GetHealthAmountToRegen();
		return ShooterCharacter->AddHealthRegen(HealthAmount);
	}

	return false;
}
