// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/Items/BaseItem.h"
#include "BaseWeapon.generated.h"

class USkeletalMeshComponent;
class UWeaponFireComponent;
class UParticleSystem;

UENUM()
enum class EWeaponType
{
	Rifle		UMETA(DisplayName="Rifle"),
	SMG			UMETA(DisplayName="SubMachine-Gun"),
	Pistol		UMETA(DisplayName="Pistol"),
	Launcher	UMETA(DisplayName="Launcher"),
	Shotgun		UMETA(DisplayName="Shotgun")
};

/**
 * 
 */
UCLASS()
class KOTHARENA_API ABaseWeapon : public ABaseItem
{
	GENERATED_BODY()

public:
	ABaseWeapon();

protected:
	virtual void BeginPlay() override;

	void SpawnHitParticle(const FVector& Location, const FRotator& Rotation) const;
	virtual void ApplyDamage(AActor* HitActor, const float Damage) const;

// Weapon interface
public:
	virtual void PullTrigger();
	virtual void ReleaseTrigger();

	UFUNCTION(BlueprintPure)
	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }

	UFUNCTION(BlueprintPure)
	FVector GetMuzzleLocation() const;

// Callbacks
protected:
	UFUNCTION()
	void OnWeaponShot(const FHitResult& ShotResult, const FVector& EndShotLocation);

	UFUNCTION()
	void OnWeaponHit(AActor* HitActor, const FVector& HitLocation, const FName& HitBoneName);
	
// Components
protected:
	UPROPERTY(VisibleAnywhere, NoClear)
	TObjectPtr<USkeletalMeshComponent> WeaponSkeletalMeshComponent;

	UPROPERTY()
	TObjectPtr<UWeaponFireComponent> WeaponFireComponent;

// Properties
protected:
	TObjectPtr<APlayerController> PlayerController;

	UPROPERTY(EditAnywhere, Category="Weapon")
	float WeaponBaseDamage = 5.f;

	UPROPERTY(EditAnywhere, Category="Weapon")
	float HeadshotMultiplier = 1.2f;

	UPROPERTY(EditAnywhere, Category="Weapon")
	TSubclassOf<UDamageType> WeaponDamageType;

	UPROPERTY(EditAnywhere, Category="Weapon")
	EWeaponType WeaponType = EWeaponType::Pistol;
	
	UPROPERTY(EditAnywhere, Category="Weapon")
	FName MuzzleSocketName = NAME_None;

	UPROPERTY(EditAnywhere, Category="Weapon|Particles")
	TObjectPtr<UParticleSystem> HitParticle;
};
