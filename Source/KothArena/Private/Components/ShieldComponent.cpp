// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/ShieldComponent.h"

#include "Characters/BaseCharacter.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UShieldComponent::UShieldComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
}


// Called when the game starts
void UShieldComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentShield = MaxShield;
	BaseCharacterRef = Cast<ABaseCharacter>(GetOwner());
}

void UShieldComponent::OnRep_CurrentShield(const float OldShieldValue)
{
	if (!BaseCharacterRef)
	{
		return;
	}

	if (OldShieldValue == CurrentShield)
	{
		// No changes
		return;
	}

	const float Amount = FMath::Max(0.f, FMath::Abs(OldShieldValue - CurrentShield));
	if (CurrentShield > OldShieldValue)
	{
		BaseCharacterRef->NotifyShieldRegen(Amount, CurrentShield);
	}
	else
	{
		BaseCharacterRef->NotifyShieldDamage(Amount, CurrentShield);
	}
}

float UShieldComponent::AbsorbDamage(const float& Damage)
{
	if (IsBroken())
	{
		return 0.f;
	}

	const float AbsorbedDamage = (CurrentShield - Damage <= 0.f)? CurrentShield : Damage; 
	CurrentShield = FMath::Max(0.f, CurrentShield - Damage);
	if (IsBroken())
	{
		OnShieldBreakDelegate.Broadcast();
	}

	return AbsorbedDamage;
}

bool UShieldComponent::RegenShield(const float& Amount)
{
	if (CurrentShield >= MaxShield)
	{
		return false;
	}
	
	CurrentShield = FMath::Min(MaxShield, CurrentShield + Amount);
	return true;
}

bool UShieldComponent::IsBroken() const
{
	return CurrentShield <= 0.f;
}

float UShieldComponent::GetMaxShield() const
{
	return MaxShield;
}

float UShieldComponent::GetCurrentShield() const
{
	return CurrentShield;
}

float UShieldComponent::GetCurrentShieldPercentage() const
{
	return GetCurrentShield() / GetMaxShield();
}

void UShieldComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UShieldComponent, CurrentShield);
}

