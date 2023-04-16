// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/WeaponFireComponent.h"

#include "Gameplay/Projectiles/BaseProjectile.h"
#include "Gameplay/Weapons/BaseWeapon.h"
#include "Kismet/GameplayStatics.h"

static TAutoConsoleVariable<bool> CVarDebugWeaponFire(
	TEXT("KothArena.WeaponFire.ShowDebugTraces"),
	false,
	TEXT("Show weapon fire traces."),
	ECVF_Default
);

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

bool UWeaponFireComponent::ComputeScreenCenterAndDirection(FVector& CenterLocation, FVector& CenterDirection) const
{
	if (PlayerController == nullptr)
	{
		return false;
	}
	
	int32 ViewportSizeX, ViewportSizeY;
	PlayerController->GetViewportSize(ViewportSizeX, ViewportSizeY);

	const FVector2D ViewportCenter = {ViewportSizeX / 2.f, ViewportSizeY / 2.f};
	return UGameplayStatics::DeprojectScreenToWorld(PlayerController, ViewportCenter, CenterLocation, CenterDirection);
}

bool UWeaponFireComponent::TraceUnderScreenCenter(FHitResult& ShotResult, FVector& TraceEndLocation) const
{
	FVector CenterLocation, CenterDirection;
	if (ComputeScreenCenterAndDirection(CenterLocation, CenterDirection))
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

		if (CVarDebugWeaponFire->GetBool())
		{
			DrawDebugDirectionalArrow(GetWorld(), WeaponRef->GetMuzzleLocation(), HitLocation, 5.f, FColor::Red, false, 10.f);
		}
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

void UWeaponFireComponent::StartConeSpreadShot()
{
	if (!WeaponRef)
	{
		return;
	}
	
	FHitResult ShotResult;
	FVector EndShotTrace, MuzzleLocation = WeaponRef->GetMuzzleLocation();
	TraceUnderScreenCenter(ShotResult, EndShotTrace);

	FVector StraightShotLocation = (EndShotTrace - MuzzleLocation);
	StraightShotLocation.Normalize();

	ShotgunShotDelegate.Broadcast(StraightShotLocation, NumOfPellets);

	for (int32 CurrentPellet = 0; CurrentPellet < NumOfPellets; ++CurrentPellet)
	{
		const float BulletSpreadOffset = BulletSpreadCurve.GetRichCurveConst()->Eval(FMath::RandRange(0.f, 1.f));
		const float RandomAngle = BulletSpreadOffset * NoiseAngle;
		const float PitchNoiseAngle = FMath::RandRange(-RandomAngle, RandomAngle);
		const float YawNoiseAngle = FMath::RandRange(-RandomAngle, RandomAngle);
		const FRotator Noise{PitchNoiseAngle, YawNoiseAngle, 0.f};

		const FVector PelletDirection = Noise.RotateVector(StraightShotLocation);
		const FVector EndPelletLocation = MuzzleLocation + (PelletDirection * GetWeaponRangeInMeters());

		FHitResult PelletHitResult;
		const bool bHitSomething = GetWorld()->LineTraceSingleByChannel(
			PelletHitResult,
			MuzzleLocation,
			EndPelletLocation,
			ECollisionChannel::ECC_Visibility
		);

		if (bHitSomething)
		{
			ShotgunPelletHitDelegate.Broadcast(PelletHitResult.GetActor(), PelletHitResult.Location, PelletHitResult.BoneName, NumOfPellets);
		}

		if (CVarDebugWeaponFire->GetBool())
		{
			DrawDebugDirectionalArrow(GetWorld(), MuzzleLocation, EndPelletLocation, 5.f, FColor::Red, false, 10.f);
		}
	}

	if (CVarDebugWeaponFire->GetBool())
	{
		const FVector IdealShotLocation = MuzzleLocation + (StraightShotLocation * GetWeaponRangeInMeters());
		DrawDebugDirectionalArrow(GetWorld(), MuzzleLocation, IdealShotLocation, 5.f, FColor::Red, false, 10.f);
	}
}

void UWeaponFireComponent::StartSpawnProjectile()
{
	if (ProjectileClass && WeaponRef)
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.Owner = GetOwner();
		
		ABaseProjectile* NewProjectile = GetWorld()->SpawnActor<ABaseProjectile>(
			ProjectileClass,
			WeaponRef->GetMuzzleLocation() + ProjectileSpawningPointOffset,
			FRotator::ZeroRotator,
			SpawnParameters
		);

		if (NewProjectile)
		{
			WeaponShotProjectileDelegate.Broadcast(NewProjectile);
			NewProjectile->OnProjectileHit().AddDynamic(this, &UWeaponFireComponent::ProjectileHitSomething);
			
			FVector CenterLocation, CenterDirection;
			if (ComputeScreenCenterAndDirection(CenterLocation, CenterDirection))
			{
				NewProjectile->Fire(CenterDirection);
			}
		}
	}
}

void UWeaponFireComponent::ProjectileHitSomething(AActor* ProjectileInstigator, AActor* OtherActor, const FHitResult& Hit)
{
	const FVector HitLocation = (ProjectileInstigator)? ProjectileInstigator->GetActorLocation() : Hit.Location;
	WeaponHitDelegate.Broadcast(OtherActor, HitLocation, Hit.BoneName);
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
	case EWeaponFireType::ConeSpread:
		StartConeSpreadShot();
		break;
	case EWeaponFireType::Projectile:
		StartSpawnProjectile();
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

