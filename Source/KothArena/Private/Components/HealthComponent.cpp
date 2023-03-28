// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/HealthComponent.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;
}


// Called every frame
void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UHealthComponent::TakeDamage(const float& Damage)
{
	if (!IsAlive())
	{
		return;
	}

	CurrentHealth = FMath::Max(0.f, CurrentHealth - Damage);
	if (!IsAlive())
	{
		OnDeathDelegate.Broadcast();
	}
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

