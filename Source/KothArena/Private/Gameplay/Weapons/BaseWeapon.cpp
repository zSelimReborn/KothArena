// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Weapons/BaseWeapon.h"

#include "Components/WeaponFireComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

ABaseWeapon::ABaseWeapon()
{
	PrimaryActorTick.bCanEverTick = true;
	WeaponSkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon Skeletal Mesh Component"));
	WeaponSkeletalMeshComponent->SetupAttachment(DefaultSceneComponent);

}

void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();

	CurrentMag = MagCapacity;
	
	PlayerController = GetWorld()->GetFirstPlayerController();
	WeaponFireComponent = FindComponentByClass<UWeaponFireComponent>();
	if (WeaponFireComponent)
	{
		WeaponFireComponent->OnWeaponShotDelegate().AddDynamic(this, &ABaseWeapon::OnWeaponShot);
		WeaponFireComponent->OnWeaponHitDelegate().AddDynamic(this, &ABaseWeapon::OnWeaponHit);
	}
}

void ABaseWeapon::SpawnHitParticle(const FVector& Location, const FRotator& Rotation) const
{
	if (HitParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, Location, Rotation);
	}
}

void ABaseWeapon::ApplyDamage(AActor* HitActor, const float Damage) const
{
	if (HitActor)
	{
		AActor* InstigatorActor = GetOwner();
		UGameplayStatics::ApplyDamage(HitActor, Damage, PlayerController, InstigatorActor, WeaponDamageType);
	}
}

void ABaseWeapon::DeductAmmo()
{
	CurrentMag = FMath::Max(0, CurrentMag - 1);
}

void ABaseWeapon::PlaySound(USoundCue* Sound, const FVector& Location, const FRotator& Rotation) const
{
	if (Sound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), Sound, Location, Rotation);
	}
}

void ABaseWeapon::PullTrigger()
{
	if (WeaponFireComponent)
	{
		if (HasAmmo())
		{
			WeaponFireComponent->StartFire();
		}
		else
		{
			PlaySound(SoundNoAmmo, GetOwner()->GetActorLocation(), GetOwner()->GetActorRotation());
		}
	}
}

void ABaseWeapon::ReleaseTrigger()
{
	if (WeaponFireComponent)
	{
		WeaponFireComponent->StopFire();
	}
}

void ABaseWeapon::Reload(const int32 Amount)
{
	CurrentMag = FMath::Min(MagCapacity, CurrentMag + Amount);
}

bool ABaseWeapon::CanShoot() const
{
	// TODO This will check also weapon state
	return HasAmmo();
}

FVector ABaseWeapon::GetMuzzleLocation() const
{
	return WeaponSkeletalMeshComponent->GetSocketLocation(MuzzleSocketName);
}

bool ABaseWeapon::HasAmmo() const
{
	return CurrentMag > 0;
}

int32 ABaseWeapon::GetMagCapacity() const
{
	return MagCapacity;
}

int32 ABaseWeapon::GetCurrentAmmo() const
{
	return CurrentMag;
}

int32 ABaseWeapon::GetMissingAmmo() const
{
	return (GetMagCapacity() - GetCurrentAmmo() >= 0)? GetMagCapacity() - GetCurrentAmmo() : GetMagCapacity();
}

void ABaseWeapon::OnWeaponShot(const FHitResult& ShotResult, const FVector& EndShotLocation)
{
	// TODO Beam emitter

	PlaySound(SoundWeaponShot, GetOwner()->GetActorLocation(), GetOwner()->GetActorRotation());
	DeductAmmo();
}

void ABaseWeapon::OnWeaponHit(AActor* HitActor, const FVector& HitLocation, const FName& HitBoneName)
{
	if (HitActor == nullptr)
	{
		return;
	}
	
	float DamageToDeal = WeaponBaseDamage;
	if (HitBoneName.IsEqual(TEXT("head")))
	{
		DamageToDeal = WeaponBaseDamage * HeadshotMultiplier;
	}

	/* DrawDebugPoint(GetWorld(), HitLocation, 10.f, FColor::Red, true);
	DrawDebugString(GetWorld(), HitLocation, HitActor->GetActorLabel(), nullptr, FColor::Red); */
	ApplyDamage(HitActor, DamageToDeal);
	SpawnHitParticle(HitLocation, FRotator::ZeroRotator);

	// TODO Different sounds based on what hit?
	PlaySound(SoundWeaponHit, HitLocation, FRotator::ZeroRotator);
}
