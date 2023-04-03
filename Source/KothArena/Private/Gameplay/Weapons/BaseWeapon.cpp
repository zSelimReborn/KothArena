// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Weapons/BaseWeapon.h"

#include "Components/WeaponFireComponent.h"
#include "Kismet/GameplayStatics.h"

ABaseWeapon::ABaseWeapon()
{
	PrimaryActorTick.bCanEverTick = true;
	WeaponSkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon Skeletal Mesh Component"));
	WeaponSkeletalMeshComponent->SetupAttachment(DefaultSceneComponent);

}

void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();

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

void ABaseWeapon::PullTrigger()
{
	if (WeaponFireComponent)
	{
		WeaponFireComponent->StartFire();
	}
}

void ABaseWeapon::ReleaseTrigger()
{
	if (WeaponFireComponent)
	{
		WeaponFireComponent->StopFire();
	}
}

FVector ABaseWeapon::GetMuzzleLocation() const
{
	return WeaponSkeletalMeshComponent->GetSocketLocation(MuzzleSocketName);
}

void ABaseWeapon::OnWeaponShot(const FHitResult& ShotResult, const FVector& EndShotLocation)
{
	// TODO Deduct ammo
	// TODO Play sound
	// TODO Beam emitter
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
}
