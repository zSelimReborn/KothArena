// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/WeaponInventoryComponent.h"

#include "Characters/BaseCharacter.h"
#include "Gameplay/Weapons/BaseWeapon.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UWeaponInventoryComponent::UWeaponInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}


// Called when the game starts
void UWeaponInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
}

// This one must be called on server
ABaseWeapon* UWeaponInventoryComponent::EquipDefaultWeapon()
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

		DefaultWeaponRef = DefaultWeapon;
		EquipWeapon(DefaultWeaponRef);
		return DefaultWeaponRef;
	}

	return nullptr;
}

void UWeaponInventoryComponent::UnEquipCurrentWeapon()
{
	if (CurrentWeaponRef)
	{
		CurrentWeaponRef->SetActorHiddenInGame(true);
	}
}

int32 UWeaponInventoryComponent::AddWeapon(ABaseWeapon* Weapon)
{
	if (WeaponInventory.Num() < InventoryCapacity)
	{
		return WeaponInventory.Add(Weapon);
	}

	if (WeaponInventory.IsValidIndex(CurrentWeaponIndex) && WeaponInventory[CurrentWeaponIndex] != nullptr)
	{
		WeaponInventory[CurrentWeaponIndex]->Destroy();
		WeaponInventory[CurrentWeaponIndex] = Weapon;
		return CurrentWeaponIndex;
	}

	return -1;
}


// Called every frame
void UWeaponInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

bool UWeaponInventoryComponent::EquipWeapon(ABaseWeapon* Weapon)
{
	if (Weapon == CurrentWeaponRef)
	{
		return false;
	}
	
	if (Weapon && Cast<ABaseCharacter>(GetOwner()))
	{
		UnEquipCurrentWeapon();
		Weapon->AttachToComponent(Cast<ABaseCharacter>(GetOwner())->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, ShooterSocketAttachName);
		CurrentWeaponRef = Weapon;
		CurrentWeaponRef->SetOwner(Cast<ABaseCharacter>(GetOwner()));
		CurrentWeaponRef->DisableCollision();
		// TODO Manage weapon collision, visibility and so on using states
		CurrentWeaponRef->OnEquip();
		CurrentWeaponIndex = AddWeapon(Weapon);

		EquipWeaponDelegate.Broadcast(CurrentWeaponRef);
		return true;
	}

	return false;
}

bool UWeaponInventoryComponent::ChangeWeapon(const int32 WeaponIndex)
{
	if (WeaponInventory.IsValidIndex(WeaponIndex) && CurrentWeaponIndex != WeaponIndex)
	{
		UnEquipCurrentWeapon();
		CurrentWeaponRef = WeaponInventory[WeaponIndex];
		CurrentWeaponIndex = WeaponIndex;
		CurrentWeaponRef->SetActorHiddenInGame(false);

		ChangeWeaponDelegate.Broadcast(CurrentWeaponRef);
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

void UWeaponInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UWeaponInventoryComponent, DefaultWeaponRef);
}

void UWeaponInventoryComponent::OnRep_DefaultWeaponRef()
{
	EquipWeapon(DefaultWeaponRef);
}

