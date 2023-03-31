// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Items/HealthItem.h"

#include "Characters/BaseCharacter.h"

int32 AHealthItem::GetHealthAmountToRegen() const
{
	return FMath::RandRange(MinHealthRegen, MaxHealthRegen);
}

bool AHealthItem::ConsumeItem(AActor* InstigatorActor)
{
	ABaseCharacter* Character = Cast<ABaseCharacter>(InstigatorActor);
	if (Character)
	{
		const int32 HealthAmount = GetHealthAmountToRegen();
		return Character->AddHealthRegen(HealthAmount);
	}

	return false;
}
