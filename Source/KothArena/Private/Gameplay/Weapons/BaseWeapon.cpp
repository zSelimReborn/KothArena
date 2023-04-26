// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Weapons/BaseWeapon.h"

#include "Characters/BaseCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/WeaponFireComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

ABaseWeapon::ABaseWeapon()
{
	PrimaryActorTick.bCanEverTick = true;
	WeaponSkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon Skeletal Mesh Component"));
	WeaponSkeletalMeshComponent->SetupAttachment(DefaultSceneComponent);

	PickupWeaponWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Pickup Widget Component"));
	PickupWeaponWidgetComponent->SetupAttachment(DefaultSceneComponent);

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Collision"));
	CapsuleComponent->SetupAttachment(DefaultSceneComponent);
}

void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();

	CurrentMag = MagCapacity;

	DisableHighlight();
	BaseCharacterRef = Cast<ABaseCharacter>(GetOwner());
	PlayerController = (BaseCharacterRef)? Cast<APlayerController>(BaseCharacterRef->GetController()) : GetWorld()->GetFirstPlayerController();
	WeaponFireComponent = FindComponentByClass<UWeaponFireComponent>();
	if (WeaponFireComponent)
	{
		WeaponFireComponent->OnWeaponShotDelegate().AddDynamic(this, &ABaseWeapon::OnWeaponShot);
		WeaponFireComponent->OnWeaponShotProjectileDelegate().AddDynamic(this, &ABaseWeapon::OnWeaponProjectileShot);
		WeaponFireComponent->OnWeaponHitDelegate().AddDynamic(this, &ABaseWeapon::OnWeaponHit);
		WeaponFireComponent->OnShotgunShotDelegate().AddDynamic(this, &ABaseWeapon::OnShotgunShot);
		WeaponFireComponent->OnShotgunPelletHitDelegate().AddDynamic(this, &ABaseWeapon::OnShotgunPelletHit);
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

void ABaseWeapon::EnableHighlight() const
{
	WeaponSkeletalMeshComponent->SetRenderCustomDepth(true);
	PickupWeaponWidgetComponent->SetVisibility(true);
}

void ABaseWeapon::DisableHighlight() const
{
	WeaponSkeletalMeshComponent->SetRenderCustomDepth(false);
	PickupWeaponWidgetComponent->SetVisibility(false);
}

void ABaseWeapon::DisableCollision() const
{
	CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABaseWeapon::OnEquip()
{
	BaseCharacterRef = Cast<ABaseCharacter>(GetOwner());
	PlayerController = (BaseCharacterRef)? Cast<APlayerController>(BaseCharacterRef->GetController()) : GetWorld()->GetFirstPlayerController();
	ItemUsedDelegate.Broadcast(this, GetOwner());
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

AController* ABaseWeapon::GetControllerOwner()
{
	if (ControllerOwner != nullptr)
	{
		return ControllerOwner;
	}

	if (BaseCharacterRef)
	{
		ControllerOwner = BaseCharacterRef->GetController();
		PlayerController = Cast<APlayerController>(BaseCharacterRef->GetController());
		AIController = Cast<AAIController>(BaseCharacterRef->GetController());
	}

	return ControllerOwner;
}

void ABaseWeapon::OnWeaponShot(const FHitResult& ShotResult, const FVector& EndShotLocation)
{
	// TODO Beam emitter

	PlaySound(SoundWeaponShot, GetOwner()->GetActorLocation(), GetOwner()->GetActorRotation());
	DeductAmmo();
}

void ABaseWeapon::OnWeaponProjectileShot(ABaseProjectile* NewProjectile)
{
	PlaySound(SoundWeaponShot, GetOwner()->GetActorLocation(), GetOwner()->GetActorRotation());
	DeductAmmo();
	// This can be a point to extend functionality. Think of a grenade launcher. 
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

	ApplyDamage(HitActor, DamageToDeal);
	SpawnHitParticle(HitLocation, FRotator::ZeroRotator);

	// TODO Different sounds based on what hit?
	PlaySound(SoundWeaponHit, HitLocation, FRotator::ZeroRotator);
}

void ABaseWeapon::OnShotgunShot(const FVector& IdealShotDirection, const int32 NumOfPellets)
{
	PlaySound(SoundWeaponShot, GetOwner()->GetActorLocation(), GetOwner()->GetActorRotation());
	DeductAmmo();
}

void ABaseWeapon::OnShotgunPelletHit(AActor* HitActor, const FVector& HitLocation, const FName& HitBoneName, const int32 NumOfPellets)
{
	if (HitActor == nullptr)
	{
		return;
	}
	
	float DamageToDeal = WeaponBaseDamage / NumOfPellets;
	if (HitBoneName.IsEqual(TEXT("head")))
	{
		DamageToDeal = WeaponBaseDamage * HeadshotMultiplier;
	}

	ApplyDamage(HitActor, DamageToDeal);
	SpawnHitParticle(HitLocation, FRotator::ZeroRotator);
}
