// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Items/ShieldItem.h"

#include "Characters/ShooterCharacter.h"

int32 AShieldItem::GetShieldAmountToRegen() const
{
	return FMath::RandRange(MinShieldRegen, MaxShieldRegen);
}

bool AShieldItem::ConsumeItem(AActor* InstigatorActor)
{
	AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(InstigatorActor);
	if (ShooterCharacter)
	{
		const int32 ShieldAmount = GetShieldAmountToRegen();
		return ShooterCharacter->AddShieldRegen(ShieldAmount);
	}

	return false;
}
