// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Gameplay/Items/BaseItem.h"
#include "Gameplay/Items/AmmoItem.h"
#include "BaseWeapon.generated.h"

class USkeletalMeshComponent;
class UWeaponFireComponent;
class UWidgetComponent;
class UCapsuleComponent;
class UParticleSystem;
class USoundCue;
class ABaseProjectile;
class ABaseCharacter;

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

	void SpawnSmokeTrail(const FVector& StartLocation, const FVector& EndLocation) const;
	
	virtual void ApplyDamage(AActor* HitActor, const float Damage);
	
	void DeductAmmo();

	/**
	 * Spawn particles and sounds just for owning client
	 */
	void HandleWeaponShot();

	/**
	 * Spawn particles and sounds just for owning client
	 */
	void HandleWeaponHit(const float BaseDamage, AActor* HitActor, const FVector& HitLocation, const FName& HitBoneName, const bool bPlaySound) const;
	
	void PlaySound(USoundCue* Sound, const FVector& Location, const FRotator& Rotation) const;

	virtual void HandlePullTrigger();
	virtual void HandleReleaseTrigger();
	virtual void HandleReload(const int32 Amount);
	
// Net functions
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	/**
	 * Spawn particles for every client and deduct ammo
	 */
	UFUNCTION(Server, Reliable)
	void ServerHandleWeaponShot();

	/**
	 * Spawn particles for every client and deals actual damage
	 */
	UFUNCTION(Server, Reliable)
	void ServerHandleWeaponHit(const float BaseDamage, AActor* HitActor, const FVector& HitLocation, const FName& HitBoneName, const bool bPlaySound);
	
	UFUNCTION(Server, Reliable)
	void ServerReload(const int32 Amount);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlaySound(USoundCue* Sound, const FVector& Location, const FRotator& Rotation) const;
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastSpawnHitParticle(const FVector& Location, const FRotator& Rotation) const;

	UFUNCTION(Server, Reliable)
	void ServerSpawnSmokeTrail(const FVector& StartLocation, const FVector& EndLocation) const;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSpawnSmokeTrail(const FVector& StartLocation, const FVector& EndLocation) const;
	
// Weapon interface
public:
	void PullTrigger();
	void ReleaseTrigger();
	void Reload(const int32 Amount);
	virtual void EnableHighlight() const override;
	virtual void DisableHighlight() const override;
	void DisableCollision() const;
	
	void OnEquip();

	UFUNCTION(BlueprintPure)
	bool CanShoot() const;
	
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

	UFUNCTION(BlueprintPure)
	FORCEINLINE ABaseCharacter* GetCharacterOwner() const { return BaseCharacterRef; };

	AController* GetControllerOwner();

// Callbacks
protected:
	UFUNCTION()
	void OnWeaponShot(const FHitResult& ShotResult, const FVector& EndShotLocation);

	UFUNCTION()
	void OnWeaponProjectileShot(ABaseProjectile* NewProjectile);

	UFUNCTION()
	void OnWeaponHit(AActor* HitActor, const FVector& HitLocation, const FName& HitBoneName);

	UFUNCTION()
	void OnShotgunShot(const FVector& IdealShotDirection, const int32 NumOfPellets);

	UFUNCTION()
	void OnShotgunPelletHit(AActor* HitActor, const FVector& HitLocation, const FName& HitBoneName, const int32 NumOfPellets);

	UFUNCTION()
	void OnShotgunPelletShot(const FHitResult& HitResult, const FVector& EndShotLocation, const int32 NumOfPellets);
	
// Components
protected:
	UPROPERTY(VisibleAnywhere, NoClear)
	TObjectPtr<USkeletalMeshComponent> WeaponSkeletalMeshComponent;

	UPROPERTY(VisibleAnywhere, NoClear)
	TObjectPtr<UWidgetComponent> PickupWeaponWidgetComponent;

	UPROPERTY(VisibleAnywhere, NoClear)
	TObjectPtr<UCapsuleComponent> CapsuleComponent;
	
	UPROPERTY()
	TObjectPtr<UWeaponFireComponent> WeaponFireComponent;

// Properties
protected:
	UPROPERTY(Transient)
	TObjectPtr<ABaseCharacter> BaseCharacterRef;

	UPROPERTY(Transient)
	TObjectPtr<AController> ControllerOwner;
	
	UPROPERTY(Transient)
	TObjectPtr<APlayerController> PlayerController;

	UPROPERTY(Transient)
	TObjectPtr<AAIController> AIController;

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

	UPROPERTY(VisibleAnywhere, Category="Weapon|Ammo", Replicated)
	int32 CurrentMag = 0;

	UPROPERTY(EditAnywhere, Category="Weapon|Ammo")
	EAmmoType AmmoType = EAmmoType::LargeCaliber;

	UPROPERTY(EditAnywhere, Category="Weapon|Sounds")
	TObjectPtr<USoundCue> SoundWeaponShot;

	UPROPERTY(EditAnywhere, Category="Weapon|Sounds")
	TObjectPtr<USoundCue> SoundWeaponHit;

	UPROPERTY(EditAnywhere, Category="Weapon|Sounds")
	TObjectPtr<USoundCue> SoundNoAmmo;

	UPROPERTY(EditAnywhere, Category="Weapon|Effects")
	TObjectPtr<UParticleSystem> SmokeTrailParticle;
};
