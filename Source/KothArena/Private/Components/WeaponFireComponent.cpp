// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/WeaponFireComponent.h"

#include "Gameplay/Weapons/BaseWeapon.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UWeaponFireComponent::UWeaponFireComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UWeaponFireComponent::BeginPlay()
{
	Super::BeginPlay();

	WeaponRef = Cast<ABaseWeapon>(GetOwner());
	PlayerController = GetWorld()->GetFirstPlayerController();
}

bool UWeaponFireComponent::TraceUnderScreenCenter(FHitResult& ShotResult, FVector& TraceEndLocation) const
{
	if (PlayerController == nullptr)
	{
		return false;
	}
	
	int32 ViewportSizeX, ViewportSizeY;
	PlayerController->GetViewportSize(ViewportSizeX, ViewportSizeY);

	const FVector2D ViewportCenter = {ViewportSizeX / 2.f, ViewportSizeY / 2.f};

	FVector CenterLocation, CenterDirection;
	if (UGameplayStatics::DeprojectScreenToWorld(PlayerController, ViewportCenter, CenterLocation, CenterDirection))
	{
		const FCollisionQueryParams ShotQueryParams{TEXT("StartSingleShot|Screen")};
		const FVector StartShotTrace = CenterLocation;
		const FVector EndShotTrace = CenterLocation + CenterDirection * GetWeaponRangeInMeters();
		const bool bHit = GetWorld()->LineTraceSingleByChannel(
			ShotResult,
			StartShotTrace,
			EndShotTrace,
			ECollisionChannel::ECC_Visibility,
			ShotQueryParams
		);

		TraceEndLocation = EndShotTrace;
		return bHit;
	}

	return false;
}

bool UWeaponFireComponent::TraceFromWeaponMuzzle(const FVector ShotEndLocation, FHitResult& ShotResult) const
{
	if (WeaponRef)
	{
		const FVector StartWeaponTrace = WeaponRef->GetMuzzleLocation();
		const FVector WeaponToCenter = (ShotEndLocation - StartWeaponTrace);
		const FVector EndWeaponTrace = StartWeaponTrace + WeaponToCenter * GetWeaponRangeInMeters();
		const FCollisionQueryParams WeaponQueryParams{TEXT("StartSingleShot|Weapon")};
		const bool bWeaponHit = GetWorld()->LineTraceSingleByChannel(
			ShotResult,
			StartWeaponTrace,
			EndWeaponTrace,
			ECollisionChannel::ECC_Visibility,
			WeaponQueryParams
		);

		return bWeaponHit;
	}

	return false;
}

void UWeaponFireComponent::StartSingleShot() const
{
	if (WeaponRef && !WeaponRef->CanShoot())
	{
		return;
	}
	
	FHitResult ShotResult;
	FHitResult WeaponMuzzleResult;
	FVector EndShotTrace;
	const bool bHit = TraceUnderScreenCenter(ShotResult, EndShotTrace);
	WeaponShotDelegate.Broadcast(ShotResult, EndShotTrace);
	if (bHit)
	{
		AActor* HitActor = ShotResult.GetActor();
		FVector HitLocation = ShotResult.Location;
		FName HitBoneName = ShotResult.BoneName;
		if (TraceFromWeaponMuzzle(EndShotTrace, WeaponMuzzleResult))
		{
			HitActor = WeaponMuzzleResult.GetActor();
			HitLocation = WeaponMuzzleResult.Location;
			HitBoneName = WeaponMuzzleResult.BoneName;
		}

		WeaponHitDelegate.Broadcast(HitActor, HitLocation, HitBoneName);
	}
}

void UWeaponFireComponent::StartAutomaticFire()
{
	GetOwner()->GetWorldTimerManager().SetTimer(AutomaticFireTimerHandle, this, &UWeaponFireComponent::StartSingleShot, AutomaticFireRate, true, 0);
}

void UWeaponFireComponent::StopAutomaticFire()
{
	GetOwner()->GetWorldTimerManager().ClearTimer(AutomaticFireTimerHandle);
}

void UWeaponFireComponent::StartBurstFire()
{
	// TODO Check if already shooting
	StartSingleShot();
	float AccumulatedBurstTime = BurstFireRate;
	for (int8 BurstBullet = 1; BurstBullet < BurstNumOfBullets; ++BurstBullet)
	{
		FTimerHandle BurstFireSingleBulletTimerHandle;
		GetOwner()->GetWorldTimerManager().SetTimer(BurstFireSingleBulletTimerHandle, this, &UWeaponFireComponent::StartSingleShot, AccumulatedBurstTime, false);
		AccumulatedBurstTime += BurstFireRate;
	}
}

void UWeaponFireComponent::StartFire()
{
	switch (WeaponFireType)
	{
	case EWeaponFireType::Burst:
		StartBurstFire();
		break;
	case EWeaponFireType::Automatic:
		StartAutomaticFire();
		break;
	case EWeaponFireType::Single:
		StartSingleShot();
		break;
	default:
		StartSingleShot();
	}
}

void UWeaponFireComponent::StopFire()
{
	if (WeaponFireType == EWeaponFireType::Automatic)
	{
		StopAutomaticFire();
	}
}

