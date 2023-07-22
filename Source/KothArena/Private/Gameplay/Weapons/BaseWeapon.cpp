// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Weapons/BaseWeapon.h"

#include "Characters/BaseCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/HighlightComponent.h"
#include "Components/WeaponFireComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"

ABaseWeapon::ABaseWeapon()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	
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
		WeaponFireComponent->OnShotgunPelletShotDelegate().AddDynamic(this, &ABaseWeapon::OnShotgunPelletShot);
	}

	if (UHighlightComponent* HighlightComponent = FindComponentByClass<UHighlightComponent>())
	{
		HighlightComponent->DisableHighlight();
	}
}

void ABaseWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseWeapon, CurrentMag);
}

void ABaseWeapon::SpawnHitParticle(const FVector& Location, const FRotator& Rotation) const
{
	if (HitParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, Location, Rotation);
	}
}

void ABaseWeapon::SpawnSmokeTrail(const FVector& StartLocation, const FVector& EndLocation) const
{
	if (SmokeTrailParticle)
	{
		UParticleSystemComponent* NewSmokeTrail = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SmokeTrailParticle, StartLocation);
		if (NewSmokeTrail)
		{
			NewSmokeTrail->SetVectorParameter(FName(TEXT("Target")), EndLocation);
		}
	}
}

void ABaseWeapon::MulticastSpawnHitParticle_Implementation(const FVector& Location, const FRotator& Rotation) const
{
	if (BaseCharacterRef && !BaseCharacterRef->IsLocallyControlled())
	{
		SpawnHitParticle(Location, Rotation);
	}
}

void ABaseWeapon::ApplyDamage(AActor* HitActor, const float Damage)
{
	if (HitActor && HasAuthority())
	{
		AActor* InstigatorActor = GetOwner();
		AController* InstigatorController = GetControllerOwner(); 
		UGameplayStatics::ApplyDamage(HitActor, Damage, InstigatorController, InstigatorActor, WeaponDamageType);
	}
}

void ABaseWeapon::DeductAmmo()
{
	CurrentMag = FMath::Max(0, CurrentMag - 1);
}

void ABaseWeapon::HandleWeaponShot()
{
	PlaySound(SoundWeaponShot, GetOwner()->GetActorLocation(), GetOwner()->GetActorRotation());
	
	if (!HasAmmo())
	{
		PlaySound(SoundNoAmmo, GetOwner()->GetActorLocation(), GetOwner()->GetActorRotation());
		ReleaseTrigger();
	}
}

void ABaseWeapon::ServerHandleWeaponShot_Implementation()
{
	MulticastPlaySound(SoundWeaponShot, GetOwner()->GetActorLocation(), GetOwner()->GetActorRotation());
	DeductAmmo();
}

void ABaseWeapon::HandleWeaponHit(const float BaseDamage, AActor* HitActor, const FVector& HitLocation, const FName& HitBoneName, const bool bPlaySound) const
{
	// TODO Different sounds based on what hit?
	SpawnHitParticle(HitLocation, FRotator::ZeroRotator);
	if (bPlaySound)
	{
		PlaySound(SoundWeaponHit, HitLocation, FRotator::ZeroRotator);
	}
}

void ABaseWeapon::ServerHandleWeaponHit_Implementation(const float BaseDamage, AActor* HitActor, const FVector& HitLocation, const FName& HitBoneName, const bool bPlaySound)
{
	if (HitActor == nullptr)
	{
		return;
	}
	
	float DamageToDeal = BaseDamage;
	if (HitBoneName.IsEqual(TEXT("head")))
	{
		DamageToDeal = BaseDamage * HeadshotMultiplier;
	}

	ApplyDamage(HitActor, DamageToDeal);
	MulticastSpawnHitParticle(HitLocation, FRotator::ZeroRotator);

	// TODO Different sounds based on what hit?
	if (bPlaySound)
	{
		MulticastPlaySound(SoundWeaponHit, HitLocation, FRotator::ZeroRotator);
	}
}

void ABaseWeapon::MulticastPlaySound_Implementation(USoundCue* Sound, const FVector& Location, const FRotator& Rotation) const
{
	if (BaseCharacterRef && !BaseCharacterRef->IsLocallyControlled())
	{
		PlaySound(Sound, Location, Rotation);
	}
}

void ABaseWeapon::PlaySound(USoundCue* Sound, const FVector& Location, const FRotator& Rotation) const
{
	if (Sound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), Sound, Location, Rotation);
	}
}

void ABaseWeapon::HandlePullTrigger()
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

void ABaseWeapon::HandleReleaseTrigger()
{
	if (WeaponFireComponent)
	{
		WeaponFireComponent->StopFire();
	}
}

void ABaseWeapon::HandleReload(const int32 Amount)
{
	CurrentMag = FMath::Min(MagCapacity, CurrentMag + Amount);
}

void ABaseWeapon::ServerReload_Implementation(const int32 Amount)
{
	HandleReload(Amount);
}

void ABaseWeapon::ServerSpawnSmokeTrail_Implementation(const FVector& StartLocation, const FVector& EndLocation) const
{
	MulticastSpawnSmokeTrail(StartLocation, EndLocation);
}

void ABaseWeapon::MulticastSpawnSmokeTrail_Implementation(const FVector& StartLocation, const FVector& EndLocation) const
{
	SpawnSmokeTrail(StartLocation, EndLocation);
}

void ABaseWeapon::PullTrigger()
{
	HandlePullTrigger();
}

void ABaseWeapon::ReleaseTrigger()
{
	HandleReleaseTrigger();
}

void ABaseWeapon::Reload(const int32 Amount)
{
	HandleReload(Amount);
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

float ABaseWeapon::GetRecoilCurrentAngle() const
{
	if (WeaponFireComponent)
	{
		return WeaponFireComponent->GetRecoilCurrentAngle();
	}

	return 0.f;
}

float ABaseWeapon::GetShotgunMaxAngle() const
{
	if (WeaponFireComponent)
	{
		if (WeaponFireComponent->GetWeaponFireType() == EWeaponFireType::ConeSpread)
		{
			return WeaponFireComponent->GetNoiseAngle();
		}
	}

	return 0.f;
}

void ABaseWeapon::OnWeaponShot(const FHitResult& ShotResult, const FVector& EndShotLocation)
{
	HandleWeaponShot();
	ServerHandleWeaponShot();
	ServerSpawnSmokeTrail(GetMuzzleLocation(), EndShotLocation);
}

void ABaseWeapon::OnWeaponProjectileShot(ABaseProjectile* NewProjectile)
{
	HandleWeaponShot();
	ServerHandleWeaponShot();
	// This can be a point to extend functionality. Think of a grenade launcher. 
}

void ABaseWeapon::OnWeaponHit(AActor* HitActor, const FVector& HitLocation, const FName& HitBoneName)
{
	HandleWeaponHit(WeaponBaseDamage, HitActor, HitLocation, HitBoneName, true);
	ServerHandleWeaponHit(WeaponBaseDamage, HitActor, HitLocation, HitBoneName, true);
}

void ABaseWeapon::OnShotgunShot(const FVector& IdealShotDirection, const int32 NumOfPellets)
{
	HandleWeaponShot();
	ServerHandleWeaponShot();
}

void ABaseWeapon::OnShotgunPelletHit(AActor* HitActor, const FVector& HitLocation, const FName& HitBoneName, const int32 NumOfPellets)
{
	const float ShotgunBaseDamage = WeaponBaseDamage / NumOfPellets;
	HandleWeaponHit(ShotgunBaseDamage, HitActor, HitLocation, HitBoneName, false);
	ServerHandleWeaponHit(ShotgunBaseDamage, HitActor, HitLocation, HitBoneName, false);
}

void ABaseWeapon::OnShotgunPelletShot(const FHitResult& HitResult, const FVector& EndShotLocation, const int32 NumOfPellets)
{
	ServerSpawnSmokeTrail(GetMuzzleLocation(), EndShotLocation);
}
