// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponFireComponent.generated.h"

#define OUT

class ABaseWeapon;
class ABaseProjectile;
class AAIController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWeaponShotDelegate, const FHitResult&, ShotResult, const FVector&, EndShotLocation);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponShotProjectileDelegate, ABaseProjectile*, NewProjectile);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnWeaponHitDelegate, AActor*, HitActor, const FVector&, HitLocation, const FName&, HitBoneName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnShotgunShotDelegate, const FVector&, IdealShotDirection, const int32, NumOfPellets);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnShotgunPelletShotDelegate, const FHitResult&, HitResult, const FVector&, EndShotLocation, const int32, NumOfPellets);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnShotgunPelletHitDelegate, AActor*, HitActor, const FVector&, HitLocation, const FName&, HitBoneName, const int32, NumOfPellets);

UENUM()
enum class EWeaponFireType
{
	Single		UMETA(DisplayName="Single Shot"),
	Burst		UMETA(DisplayName="Burst"),
	Automatic	UMETA(DisplayName="Automatic"),
	ConeSpread	UMETA(DisplayName="Cone Spread"),
	Projectile	UMETA(DisplayName="Projectile")
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

	AActor* GetOwnerToIgnore() const;
	
	bool ComputeScreenCenterAndDirection(OUT FVector& CenterLocation, OUT FVector& CenterDirection) const;
	bool TraceUnderScreenCenter(OUT FHitResult& ShotResult, OUT FVector& TraceEndLocation, bool bShouldUseRecoil) const;
	bool TraceFromWeaponMuzzle(const FVector ShotEndLocation, OUT FHitResult& ShotResult) const;
	
	void StartSingleShot();
	void StartAutomaticFire();
	void StopAutomaticFire();
	void StartBurstFire();
	void StartConeSpreadShot();
	void StartSpawnProjectile();
	
	void HandleStartFire();
	void HandleStopFire();

	void AddRecoil();
	FVector ComputeDirectionUsingRecoil(const FVector& ShotDirection) const;
	void UpdateRecoil(const float DeltaTime);

// Callback
protected:
	UFUNCTION()
	void ProjectileHitSomething(AActor* ProjectileInstigator, AActor* OtherActor, const FHitResult& Hit);

	UFUNCTION()
	void OnFinishBurstFire();
	
public:	
	void StartFire();
	void StopFire();

	FOnWeaponShotDelegate& OnWeaponShotDelegate() { return WeaponShotDelegate; }
	FOnWeaponShotProjectileDelegate& OnWeaponShotProjectileDelegate() { return WeaponShotProjectileDelegate; }
	FOnWeaponHitDelegate& OnWeaponHitDelegate() { return WeaponHitDelegate; }
	FOnShotgunShotDelegate& OnShotgunShotDelegate() { return ShotgunShotDelegate; }
	FOnShotgunPelletHitDelegate& OnShotgunPelletHitDelegate() { return ShotgunPelletHitDelegate; }
	FOnShotgunPelletShotDelegate& OnShotgunPelletShotDelegate() { return ShotgunPelletShotDelegate; }
	
	FORCEINLINE float GetWeaponRangeInMeters() const { return WeaponRange * 100.f; }
	FORCEINLINE EWeaponFireType GetWeaponFireType() const { return WeaponFireType; }

	AController* FillControllerOwner();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
// Properties
protected:
	UPROPERTY(Transient)
	TObjectPtr<ABaseWeapon> WeaponRef;

	UPROPERTY(Transient)
	TObjectPtr<AController> ControllerRef;

	UPROPERTY(Transient)
	TObjectPtr<APlayerController> PlayerControllerRef;

	UPROPERTY(Transient)
	TObjectPtr<AAIController> AIControllerRef;
	
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

	UPROPERTY(EditAnywhere, Category="Weapon Fire|Shotgun")
	int32 NumOfPellets = 25;

	UPROPERTY(EditAnywhere, Category="Weapon Fire|Shotgun")
	float NoiseAngle = 25.f;

	UPROPERTY(EditAnywhere, Category="Weapon Fire|Shotgun")
	FRuntimeFloatCurve BulletSpreadCurve;

	UPROPERTY(EditAnywhere, Category="Weapon Fire|Projectile")
	TSubclassOf<ABaseProjectile> ProjectileClass;
	
	UPROPERTY(EditAnywhere, Category="Weapon Fire|Projectile")
	FVector ProjectileSpawningPointOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Category="Weapon Fire|Recoil")
	bool bRecoilEnabled = false;

	UPROPERTY(EditAnywhere, Category="Weapon Fire|Recoil")
	float RecoilMaxAngle = 10.f;
	
	UPROPERTY(EditAnywhere, Category="Weapon Fire|Recoil")
	float RecoilVelocity = 0.1f;
	
	UPROPERTY(VisibleAnywhere, Category="Weapon Fire|Recoil")
	float RecoilCurrentAngle = 0.f;

	// TODO: will use states from owner weapon
	UPROPERTY(VisibleAnywhere, Category="Weapon Fire")
	bool bIsShooting = false;
	
	FTimerHandle AutomaticFireTimerHandle;

// Delegates
protected:
	FOnWeaponShotDelegate WeaponShotDelegate;

	FOnWeaponHitDelegate WeaponHitDelegate;

	FOnWeaponShotProjectileDelegate WeaponShotProjectileDelegate;

	FOnShotgunShotDelegate ShotgunShotDelegate;

	FOnShotgunPelletHitDelegate ShotgunPelletHitDelegate;

	FOnShotgunPelletShotDelegate ShotgunPelletShotDelegate;
};
