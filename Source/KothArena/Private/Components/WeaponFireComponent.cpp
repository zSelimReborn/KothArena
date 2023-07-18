// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/WeaponFireComponent.h"

#include "Characters/BaseCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Gameplay/Projectiles/BaseProjectile.h"
#include "Gameplay/Weapons/BaseWeapon.h"
#include "Kismet/GameplayStatics.h"
#include "Utils/PlayerUtils.h"

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
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}


// Called when the game starts
void UWeaponFireComponent::BeginPlay()
{
	Super::BeginPlay();

	WeaponRef = Cast<ABaseWeapon>(GetOwner());
}

AActor* UWeaponFireComponent::GetOwnerToIgnore() const
{
	if (WeaponRef)
	{
		return WeaponRef->GetCharacterOwner();
	}

	return nullptr;
}

bool UWeaponFireComponent::ComputeScreenCenterAndDirection(FVector& CenterLocation, FVector& CenterDirection) const
{
	return UPlayerUtils::ComputeScreenCenterAndDirection(PlayerControllerRef, CenterLocation, CenterDirection);
}

bool UWeaponFireComponent::TraceUnderScreenCenter(FHitResult& ShotResult, FVector& TraceEndLocation, bool bShouldUseRecoil) const
{
	FVector CenterLocation, CenterDirection;
	if (ComputeScreenCenterAndDirection(CenterLocation, CenterDirection))
	{
		if (bRecoilEnabled && bShouldUseRecoil)
		{
			CenterDirection = ComputeDirectionUsingRecoil(CenterDirection);	
		}
		
		FCollisionQueryParams ShotQueryParams{TEXT("StartSingleShot|Screen")};
		const FVector StartShotTrace = CenterLocation;
		const FVector EndShotTrace = CenterLocation + CenterDirection * GetWeaponRangeInMeters();

		ShotQueryParams.AddIgnoredActor(GetOwnerToIgnore());
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
		FCollisionQueryParams WeaponQueryParams{TEXT("StartSingleShot|Weapon")};
		WeaponQueryParams.AddIgnoredActor(GetOwnerToIgnore());
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

void UWeaponFireComponent::StartSingleShot()
{
	if (WeaponRef && !WeaponRef->CanShoot())
	{
		bIsShooting = false;
		return;
	}

	AActor* HitActor = nullptr;
	FVector HitLocation, ShotEndLocation;
	FName HitBoneName;
	FHitResult HitResult, WeaponShotResult;
	bool bHitSomething = false;

	AddRecoil();
	if (TraceUnderScreenCenter(HitResult, ShotEndLocation, true))
	{
		HitActor = HitResult.GetActor();
		HitLocation = HitResult.Location;
		HitBoneName = HitResult.BoneName;
		ShotEndLocation = HitLocation;
		bHitSomething = true;
	}
	
	if (TraceFromWeaponMuzzle(ShotEndLocation, WeaponShotResult))
	{
		if (HitActor != WeaponShotResult.GetActor())
		{
			HitActor = WeaponShotResult.GetActor();
			HitLocation = WeaponShotResult.Location;
			HitBoneName = WeaponShotResult.BoneName;
			ShotEndLocation = HitLocation;
			HitResult = WeaponShotResult;
			bHitSomething = true;
		}
	}

	if (bHitSomething)
	{
		WeaponHitDelegate.Broadcast(HitActor, HitLocation, HitBoneName);
	}

	WeaponShotDelegate.Broadcast(HitResult, ShotEndLocation);

	if (CVarDebugWeaponFire->GetBool())
	{
		DrawDebugDirectionalArrow(GetWorld(), WeaponRef->GetMuzzleLocation(), ShotEndLocation, 5.f, FColor::Red, false, 10.f);
	}
}

void UWeaponFireComponent::StartAutomaticFire()
{
	bIsShooting = true;
	GetOwner()->GetWorldTimerManager().SetTimer(AutomaticFireTimerHandle, this, &UWeaponFireComponent::StartSingleShot, AutomaticFireRate, true, 0);
}

void UWeaponFireComponent::StopAutomaticFire()
{
	GetOwner()->GetWorldTimerManager().ClearTimer(AutomaticFireTimerHandle);
}

void UWeaponFireComponent::StartBurstFire()
{
	if (bIsShooting)
	{
		return;
	}

	bIsShooting = true;
	StartSingleShot();
	float AccumulatedBurstTime = BurstFireRate;
	for (int8 BurstBullet = 1; BurstBullet < BurstNumOfBullets; ++BurstBullet)
	{
		FTimerHandle BurstFireSingleBulletTimerHandle;
		GetOwner()->GetWorldTimerManager().SetTimer(BurstFireSingleBulletTimerHandle, this, &UWeaponFireComponent::StartSingleShot, AccumulatedBurstTime, false);
		AccumulatedBurstTime += BurstFireRate;
	}

	FTimerHandle FinishBurstFireTimerHandle;
	GetOwner()->GetWorldTimerManager().SetTimer(FinishBurstFireTimerHandle, this, &UWeaponFireComponent::OnFinishBurstFire, AccumulatedBurstTime, false);
}

void UWeaponFireComponent::StartConeSpreadShot()
{
	if (!WeaponRef)
	{
		return;
	}
	
	FHitResult ShotResult;
	FVector EndShotTrace, MuzzleLocation = WeaponRef->GetMuzzleLocation();
	AddRecoil();
	TraceUnderScreenCenter(ShotResult, EndShotTrace, false);

	FVector StraightShotLocation = (EndShotTrace - MuzzleLocation);
	StraightShotLocation.Normalize();

	ShotgunShotDelegate.Broadcast(StraightShotLocation, NumOfPellets);

	for (int32 CurrentPellet = 0; CurrentPellet < NumOfPellets; ++CurrentPellet)
	{
		const float BulletSpreadOffset = BulletSpreadCurve.GetRichCurveConst()->Eval(FMath::RandRange(0.f, 1.f));
		const float RandomAngle = BulletSpreadOffset * (NoiseAngle + RecoilCurrentAngle);
		const float PitchNoiseAngle = FMath::RandRange(-RandomAngle, RandomAngle);
		const float YawNoiseAngle = FMath::RandRange(-RandomAngle, RandomAngle);
		const FRotator Noise{PitchNoiseAngle, YawNoiseAngle, 0.f};

		const FVector PelletDirection = Noise.RotateVector(StraightShotLocation);
		const FVector EndPelletLocation = MuzzleLocation + (PelletDirection * GetWeaponRangeInMeters());
		FVector EndShotLocation = EndPelletLocation;

		FCollisionQueryParams ShotgunQueryParams{TEXT("StartConeSpreadShot")};
		ShotgunQueryParams.AddIgnoredActor(GetOwnerToIgnore());
		FHitResult PelletHitResult;
		const bool bHitSomething = GetWorld()->LineTraceSingleByChannel(
			PelletHitResult,
			MuzzleLocation,
			EndPelletLocation,
			ECollisionChannel::ECC_Visibility,
			ShotgunQueryParams
		);

		if (bHitSomething)
		{
			EndShotLocation = PelletHitResult.Location;
			ShotgunPelletHitDelegate.Broadcast(PelletHitResult.GetActor(), PelletHitResult.Location, PelletHitResult.BoneName, NumOfPellets);
		}
		
		ShotgunPelletShotDelegate.Broadcast(PelletHitResult, EndShotLocation, NumOfPellets);

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
		const FTransform ProjectileTransform{FRotator::ZeroRotator, WeaponRef->GetMuzzleLocation() + ProjectileSpawningPointOffset};
		
		ABaseProjectile* NewProjectile = Cast<ABaseProjectile>(
			UGameplayStatics::BeginDeferredActorSpawnFromClass(
				this,
				ProjectileClass,
				ProjectileTransform,
				ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn,
				GetOwner()
			)
		);

		if (NewProjectile)
		{
			NewProjectile->GetTriggerVolume()->MoveIgnoreActors.Add(GetOwnerToIgnore());
			NewProjectile->GetTriggerVolume()->MoveIgnoreActors.Add(WeaponRef);

			UGameplayStatics::FinishSpawningActor(NewProjectile, ProjectileTransform);
			
			WeaponShotProjectileDelegate.Broadcast(NewProjectile);
			NewProjectile->OnProjectileHit().AddDynamic(this, &UWeaponFireComponent::ProjectileHitSomething);
			
			FVector CenterLocation, CenterDirection;
			if (ComputeScreenCenterAndDirection(CenterLocation, CenterDirection))
			{
				const FVector CenterDestination = CenterLocation + CenterDirection * GetWeaponRangeInMeters();
				FVector ToCenter = CenterDestination - ProjectileTransform.GetLocation();
				ToCenter.Normalize();
				
				NewProjectile->Fire(ToCenter);
			}
		}
	}
}

void UWeaponFireComponent::HandleStartFire()
{
	FillControllerOwner();
	
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
	default:
		StartSingleShot();
	}
}

void UWeaponFireComponent::HandleStopFire()
{
	if (WeaponFireType == EWeaponFireType::Automatic)
	{
		StopAutomaticFire();
	}

	// Burst gets handled by another timer. You cannot spam burst fire
	if (WeaponFireType != EWeaponFireType::Burst)
	{
		bIsShooting = false;
	}
}

void UWeaponFireComponent::AddRecoil()
{
	RecoilCurrentAngle = FMath::Clamp(RecoilCurrentAngle + RecoilVelocity, 0.f, RecoilMaxAngle);
}

FVector UWeaponFireComponent::ComputeDirectionUsingRecoil(const FVector& ShotDirection) const
{
	const FVector ShotDirectionNormalized = ShotDirection.GetSafeNormal();
	FVector ShotAfterRecoil = ShotDirection;
	
	const float PitchNoise = FMath::RandRange(-RecoilCurrentAngle, RecoilCurrentAngle);
	const float YawNoise = FMath::RandRange(-RecoilCurrentAngle, RecoilCurrentAngle);
	const FRotator RecoilNoise{PitchNoise, YawNoise, 0.f};
	ShotAfterRecoil = RecoilNoise.RotateVector(ShotDirectionNormalized);
	
	return ShotAfterRecoil;
}

void UWeaponFireComponent::UpdateRecoil(const float DeltaTime)
{
	if (!bIsShooting && bRecoilEnabled)
	{
		RecoilCurrentAngle = FMath::Clamp(RecoilCurrentAngle - DeltaTime, 0.f, RecoilMaxAngle);
	}
}

void UWeaponFireComponent::ProjectileHitSomething(AActor* ProjectileInstigator, AActor* OtherActor, const FHitResult& Hit)
{
	const FVector HitLocation = (ProjectileInstigator)? ProjectileInstigator->GetActorLocation() : Hit.Location;
	WeaponHitDelegate.Broadcast(OtherActor, HitLocation, Hit.BoneName);
}

void UWeaponFireComponent::OnFinishBurstFire()
{
	bIsShooting = false;
}

void UWeaponFireComponent::StartFire()
{
	HandleStartFire();
}

void UWeaponFireComponent::StopFire()
{
	HandleStopFire();
}

AController* UWeaponFireComponent::FillControllerOwner()
{
	if (ControllerRef != nullptr)
	{
		return ControllerRef;
	}

	ControllerRef = (WeaponRef)? WeaponRef->GetControllerOwner() : nullptr;
	PlayerControllerRef = Cast<APlayerController>(ControllerRef);
	AIControllerRef = Cast<AAIController>(ControllerRef);
	return ControllerRef;
}

void UWeaponFireComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateRecoil(DeltaTime);
}
