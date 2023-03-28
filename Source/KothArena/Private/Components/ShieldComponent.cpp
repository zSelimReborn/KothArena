// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/ShieldComponent.h"

// Sets default values for this component's properties
UShieldComponent::UShieldComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UShieldComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentShield = MaxShield;
}


// Called every frame
void UShieldComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UShieldComponent::AbsorbDamage(const float& Damage)
{
	if (IsBroken())
	{
		return;
	}

	CurrentShield = FMath::Max(0.f, CurrentShield - Damage);
	if (IsBroken())
	{
		OnShieldBreakDelegate.Broadcast();
	}
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

