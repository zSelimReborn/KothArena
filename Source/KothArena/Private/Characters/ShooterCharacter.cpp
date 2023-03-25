// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/ShooterCharacter.h"

#include "Components/ShieldComponent.h"

AShooterCharacter::AShooterCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ShieldComponent = CreateDefaultSubobject<UShieldComponent>(TEXT("Shield Component"));
}

float AShooterCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	bool bShouldDamageHealth = false;
	float RemainingDamage = DamageAmount;
	
	if (!ShieldComponent->IsBroken())
	{
		RemainingDamage = ShieldComponent->GetCurrentShield() - DamageAmount;
		if (RemainingDamage < 0.f)
		{
			bShouldDamageHealth = true;
			RemainingDamage = FMath::Abs(RemainingDamage);
		}

		ShieldComponent->AbsorbDamage(DamageAmount);
		if (ShieldComponent->IsBroken())
		{
			OnShieldBroken();
		}
	}

	if (bShouldDamageHealth || ShieldComponent->IsBroken())
	{
		Super::TakeDamage(RemainingDamage, DamageEvent, EventInstigator, DamageCauser);
	}

	return DamageAmount;
}

float AShooterCharacter::GetMaxShield() const
{
	return ShieldComponent->GetMaxShield();
}

float AShooterCharacter::GetCurrentShield() const
{
	return ShieldComponent->GetCurrentShield();
}

void AShooterCharacter::OnShieldBroken()
{
}
