// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/WeaponInventoryComponent.h"

#include "Characters/BaseCharacter.h"
#include "Gameplay/Weapons/BaseWeapon.h"

// Sets default values for this component's properties
UWeaponInventoryComponent::UWeaponInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UWeaponInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	BaseCharacterRef = Cast<ABaseCharacter>(GetOwner());
	if (bEquipDefaultWeaponOnBegin)
	{
		EquipDefaultWeapon();
	}
}

bool UWeaponInventoryComponent::EquipDefaultWeapon()
{
	if (DefaultWeaponClass)
	{
		FActorSpawnParameters WeaponSpawnParams{};
		WeaponSpawnParams.Owner = GetOwner();
	
		ABaseWeapon* DefaultWeapon = GetWorld()->SpawnActor<ABaseWeapon>(
			DefaultWeaponClass,
			GetOwner()->GetActorLocation(),
			GetOwner()->GetActorRotation(),
			WeaponSpawnParams
		);

		return EquipWeapon(DefaultWeapon);
	}

	return false;
}


// Called every frame
void UWeaponInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

bool UWeaponInventoryComponent::EquipWeapon(ABaseWeapon* Weapon)
{
	if (Weapon && BaseCharacterRef)
	{
		if (CurrentWeaponRef)
		{
			// TODO Remove from hand old current weapon
		}

		Weapon->AttachToComponent(BaseCharacterRef->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, ShooterSocketAttachName);
		CurrentWeaponRef = Weapon;
		WeaponInventory.Add(Weapon);
		return true;
	}

	return false;
}

void UWeaponInventoryComponent::PullTrigger()
{
	if (CurrentWeaponRef)
	{
		CurrentWeaponRef->PullTrigger();
	}
}

void UWeaponInventoryComponent::ReleaseTrigger()
{
	if (CurrentWeaponRef)
	{
		CurrentWeaponRef->ReleaseTrigger();
	}
}

