// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponFireComponent.generated.h"

#define OUT

class ABaseWeapon;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWeaponShotDelegate, const FHitResult&, ShotResult, const FVector&, EndShotLocation);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnWeaponHitDelegate, AActor*, HitActor, const FVector&, HitLocation, const FName&, HitBoneName);

UENUM()
enum class EWeaponFireType
{
	Single		UMETA(DisplayName="Single Shot"),
	Burst		UMETA(DisplayName="Burst"),
	Automatic	UMETA(DisplayName="Automatic")
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KOTHARENA_API UWeaponFireComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWeaponFireComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	bool TraceUnderScreenCenter(OUT FHitResult& ShotResult, OUT FVector& TraceEndLocation) const;
	bool TraceFromWeaponMuzzle(const FVector ShotEndLocation, OUT FHitResult& ShotResult) const;
	
	void StartSingleShot() const;
	void StartAutomaticFire();
	void StopAutomaticFire();
	void StartBurstFire();

public:	
	void StartFire();
	void StopFire();

	FOnWeaponShotDelegate& OnWeaponShotDelegate() { return WeaponShotDelegate; }
	FOnWeaponHitDelegate& OnWeaponHitDelegate() { return WeaponHitDelegate; }
	
	FORCEINLINE float GetWeaponRangeInMeters() const { return WeaponRange * 100.f; }
	FORCEINLINE EWeaponFireType GetWeaponFireType() const { return WeaponFireType; }

// Properties
protected:
	UPROPERTY(Transient)
	TObjectPtr<ABaseWeapon> WeaponRef;

	TObjectPtr<APlayerController> PlayerController;
	
	UPROPERTY(EditAnywhere, Category="Weapon Fire")
	EWeaponFireType WeaponFireType = EWeaponFireType::Single;

	UPROPERTY(EditAnywhere, Category="Weapon Fire", meta=(ForceUnits="m"))
	float WeaponRange = 100.f;

	UPROPERTY(EditAnywhere, Category="Weapon Fire|Burst")
	int8 BurstNumOfBullets = 3;
	
	UPROPERTY(EditAnywhere, Category="Weapon Fire|Burst")
	float BurstFireRate = 0.2f;

	UPROPERTY(EditAnywhere, Category="Weapon Fire|Automatic")
	float AutomaticFireRate = 0.2f;
	
	FTimerHandle AutomaticFireTimerHandle;

// Delegates
protected:
	FOnWeaponShotDelegate WeaponShotDelegate;

	FOnWeaponHitDelegate WeaponHitDelegate;
};
