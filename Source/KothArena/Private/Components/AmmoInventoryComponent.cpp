// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/AmmoInventoryComponent.h"

// Sets default values for this component's properties
UAmmoInventoryComponent::UAmmoInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UAmmoInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	FillZeroAmmo();
}

void UAmmoInventoryComponent::FillZeroAmmo()
{
	if (!AmmoInventory.Contains(EAmmoType::LargeCaliber))
	{
		AmmoInventory.Add(EAmmoType::LargeCaliber, 0);
	}

	if (!AmmoInventory.Contains(EAmmoType::SmallCaliber))
	{
		AmmoInventory.Add(EAmmoType::SmallCaliber, 0);
	}

	if (!AmmoInventory.Contains(EAmmoType::Missile))
	{
		AmmoInventory.Add(EAmmoType::Missile, 0);
	}

	if (!AmmoInventory.Contains(EAmmoType::Shotgun))
	{
		AmmoInventory.Add(EAmmoType::Shotgun, 0);
	}
}

void UAmmoInventoryComponent::AddAmmo(const EAmmoType AmmoType, const int32 Amount)
{
	if (AmmoInventory.Contains(AmmoType))
	{
		AmmoInventory[AmmoType] += Amount;
	}
	else
	{
		AmmoInventory.Add(AmmoType, Amount);
	}
}

bool UAmmoInventoryComponent::HasAmmo(const EAmmoType AmmoType, const int32 Amount)
{
	if (!AmmoInventory.Contains(AmmoType))
	{
		return false;
	}

	return AmmoInventory[AmmoType] >= Amount;
}

int32 UAmmoInventoryComponent::UseAmmo(const EAmmoType AmmoType, const int32 Amount)
{
	if (!AmmoInventory.Contains(AmmoType))
	{
		return 0;
	}

	const int32 ActualUsedAmmo = (AmmoInventory[AmmoType] - Amount <= 0)? AmmoInventory[AmmoType] : Amount;
	AmmoInventory[AmmoType] -= ActualUsedAmmo;
	return ActualUsedAmmo;
}

int32 UAmmoInventoryComponent::GetAmmoStorage(const EAmmoType AmmoType)
{
	if (!AmmoInventory.Contains(AmmoType))
	{
		return 0;
	}

	return AmmoInventory[AmmoType];
}

