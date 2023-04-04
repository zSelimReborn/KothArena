// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Items/AmmoItem.h"

#include "Characters/BaseCharacter.h"

int32 AAmmoItem::GetAmountToAdd() const
{
	return FMath::RandRange(MinAmount, MaxAmount);
}

bool AAmmoItem::ConsumeItem(AActor* InstigatorActor)
{
	ABaseCharacter* Character = Cast<ABaseCharacter>(InstigatorActor);
	if (Character)
	{
		const int32 Amount = GetAmountToAdd();
		return Character->AddAmmo(AmmoType, Amount);
	}

	return false;
}


