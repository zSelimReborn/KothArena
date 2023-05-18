// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/HealthComponent.h"

#include "Characters/BaseCharacter.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UHealthComponent::OnRegister()
{
	Super::OnRegister();

	if (GetOwner()->HasAuthority())
	{
		CurrentHealth = MaxHealth;
	}
	
	BaseCharacterRef = Cast<ABaseCharacter>(GetOwner());	
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UHealthComponent, CurrentHealth);
}

void UHealthComponent::OnRep_CurrentHealth(const float OldHealthValue)
{
	if (!BaseCharacterRef || !BaseCharacterRef->IsLocallyControlled())
	{
		return;
	}
	
	if (OldHealthValue == CurrentHealth)
	{
		return;
	}

	const float Amount = FMath::Max(0.f, FMath::Abs(OldHealthValue - CurrentHealth));
	if (CurrentHealth > OldHealthValue)
	{
		BaseCharacterRef->NotifyHealthRegen(Amount, CurrentHealth);
	}
	else
	{
		BaseCharacterRef->NotifyHealthDamage(Amount, CurrentHealth);
	}
}

float UHealthComponent::TakeDamage(const float& Damage)
{
	if (!IsAlive())
	{
		return 0.f;
	}

	const float TakenDamage = (CurrentHealth - Damage <= 0.f)? CurrentHealth : Damage;
	CurrentHealth = FMath::Max(0.f, CurrentHealth - Damage);
	if (!IsAlive())
	{
		OnDeathDelegate.Broadcast();
	}

	return TakenDamage;
}

bool UHealthComponent::RegenHealth(const float& Amount)
{
	if (!IsAlive())
	{
		return false;
	}

	if (CurrentHealth >= MaxHealth)
	{
		return false;
	}
	
	CurrentHealth = FMath::Min(MaxHealth, CurrentHealth + Amount);
	return true;
}

bool UHealthComponent::IsAlive() const
{
	return CurrentHealth > 0.f;
}

float UHealthComponent::GetCurrentHealth() const
{
	return CurrentHealth;
}

float UHealthComponent::GetMaxHealth() const
{
	return MaxHealth;
}

float UHealthComponent::GetCurrentHealthPercentage() const
{
	return GetCurrentHealth() / GetMaxHealth();
}

