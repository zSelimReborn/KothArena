// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Items/ShieldItem.h"

#include "Characters/BaseCharacter.h"

int32 AShieldItem::GetShieldAmountToRegen() const
{
	return FMath::RandRange(MinShieldRegen, MaxShieldRegen);
}

bool AShieldItem::ConsumeItem(AActor* InstigatorActor)
{
	ABaseCharacter* Character = Cast<ABaseCharacter>(InstigatorActor);
	if (Character)
	{
		const int32 ShieldAmount = GetShieldAmountToRegen();
		return Character->AddShieldRegen(ShieldAmount);
	}

	return false;
}
