// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/Items/BaseItem.h"
#include "BaseWeapon.generated.h"

class USkeletalMeshComponent;
class UWeaponFireComponent;
class UParticleSystem;
class USoundCue;

UENUM()
enum class EWeaponType
{
	Rifle		UMETA(DisplayName="Rifle"),
	SMG			UMETA(DisplayName="SubMachine-Gun"),
	Pistol		UMETA(DisplayName="Pistol"),
	Launcher	UMETA(DisplayName="Launcher"),
	Shotgun		UMETA(DisplayName="Shotgun"),
	Sniper		UMETA(DisplayName="Sniper")
};

// TODO Move elsewhere
UENUM()
enum class EAmmoType
{
	LargeCaliber	UMETA(DisplayName="Large Caliber"),
	SmallCaliber	UMETA(DisplayName="Small Caliber"),
	Shotgun			UMETA(DisplayName="Shot shell"),
	Missile			UMETA(DisplayName="Missile")
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
	void DeductAmmo();
	void PlaySound(USoundCue* Sound, const FVector& Location, const FRotator& Rotation) const;

// Weapon interface
public:
	virtual void PullTrigger();
	virtual void ReleaseTrigger();
	virtual void Reload(const int32 Amount);

	UFUNCTION(BlueprintPure)
	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE EAmmoType GetAmmoType() const { return AmmoType; }

	UFUNCTION(BlueprintPure)
	FVector GetMuzzleLocation() const;

	UFUNCTION(BlueprintPure)
	bool HasAmmo() const;

	UFUNCTION(BlueprintPure)
	int32 GetMagCapacity() const;

	UFUNCTION(BlueprintPure)
	int32 GetCurrentAmmo() const;

	UFUNCTION(BlueprintPure)
	int32 GetMissingAmmo() const;

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

	UPROPERTY(EditAnywhere, Category="Weapon|Ammo")
	int32 MagCapacity = 25;

	UPROPERTY(VisibleAnywhere, Category="Weapon|Ammo")
	int32 CurrentMag = 0;

	UPROPERTY(EditAnywhere, Category="Weapon|Ammo")
	EAmmoType AmmoType = EAmmoType::LargeCaliber;

	UPROPERTY(EditAnywhere, Category="Weapon|Sounds")
	TObjectPtr<USoundCue> SoundWeaponShot;

	UPROPERTY(EditAnywhere, Category="Weapon|Sounds")
	TObjectPtr<USoundCue> SoundWeaponHit;

	UPROPERTY(EditAnywhere, Category="Weapon|Sounds")
	TObjectPtr<USoundCue> SoundNoAmmo;
};
