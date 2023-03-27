// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/ShooterCharacter.h"

#include "Components/HealthComponent.h"
#include "Components/ShieldComponent.h"
#include "UI/PlayerHud.h"

AShooterCharacter::AShooterCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ShieldComponent = CreateDefaultSubobject<UShieldComponent>(TEXT("Shield Component"));
}

void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	PC = Cast<APlayerController>(GetController());
	if (PlayerHudClass)
	{
		PlayerHudRef = CreateWidget<UPlayerHud>(PC, PlayerHudClass);
		PlayerHudRef->AddToViewport();
		PlayerHudRef->SetCurrentHealth(HealthComponent->GetCurrentHealth());
		PlayerHudRef->SetMaxHealth(HealthComponent->GetMaxHealth());
		PlayerHudRef->SetCurrentShield(ShieldComponent->GetCurrentShield());
		PlayerHudRef->SetMaxShield(ShieldComponent->GetMaxShield());
	}
}

float AShooterCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	bool bShouldDamageHealth = false;
	float RemainingDamage = DamageAmount;
	float DamageAbsorbedByShield = 0.f;
	float DamageAbsorbedByHealth = 0.f;

	if (!HealthComponent->IsAlive())
	{
		return 0.f;
	}
	
	if (!ShieldComponent->IsBroken())
	{
		RemainingDamage = ShieldComponent->GetCurrentShield() - DamageAmount;
		if (RemainingDamage < 0.f)
		{
			bShouldDamageHealth = true;
			RemainingDamage = FMath::Abs(RemainingDamage);
		}

		DamageAbsorbedByShield = ((ShieldComponent->GetCurrentShield() - DamageAmount) < 0.f)? ShieldComponent->GetCurrentShield() : DamageAmount;
		ShieldComponent->AbsorbDamage(DamageAmount);
		if (ShieldComponent->IsBroken())
		{
			OnShieldBroken();
		}
	}

	if (bShouldDamageHealth || ShieldComponent->IsBroken())
	{
		DamageAbsorbedByHealth = Super::TakeDamage(RemainingDamage, DamageEvent, EventInstigator, DamageCauser);
	}

	const float TotalDamageAbsorbed = DamageAbsorbedByShield + DamageAbsorbedByHealth;
	if (PlayerHudRef)
	{
		PlayerHudRef->OnTakeDamage(TotalDamageAbsorbed, HealthComponent->GetCurrentHealth(), ShieldComponent->GetCurrentShield());
	}
	
	return TotalDamageAbsorbed;
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
