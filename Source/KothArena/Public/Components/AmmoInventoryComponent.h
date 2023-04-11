// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Gameplay/Weapons/BaseWeapon.h"
#include "AmmoInventoryComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KOTHARENA_API UAmmoInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAmmoInventoryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void FillZeroAmmo();

// Ammo Inventory interface
public:	
	void AddAmmo(const EAmmoType AmmoType, const int32 Amount);
	bool HasAmmo(const EAmmoType AmmoType, const int32 Amount);
	int32 UseAmmo(const EAmmoType AmmoType, const int32 Amount);
	int32 GetAmmoStorage(const EAmmoType AmmoType);
	
// Properties
protected:
	UPROPERTY(EditAnywhere, Category="Ammo")
	TMap<EAmmoType, int32> AmmoInventory;
};
