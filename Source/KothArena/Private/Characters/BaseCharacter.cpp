// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BaseCharacter.h"

#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Components/AmmoInventoryComponent.h"
#include "Components/HealthComponent.h"
#include "Components/SearchItemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/ShieldComponent.h"
#include "Components/WeaponInventoryComponent.h"
#include "UI/PlayerHud.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Boom"));
	CameraBoom->SetupAttachment(GetRootComponent());

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera Component"));
	CameraComponent->SetupAttachment(CameraBoom);

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("Health Component"));
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	ShieldComponent = FindComponentByClass<UShieldComponent>();
	WeaponInventoryComponent = FindComponentByClass<UWeaponInventoryComponent>();
	AmmoInventoryComponent = FindComponentByClass<UAmmoInventoryComponent>();
	SearchItemComponent = FindComponentByClass<USearchItemComponent>();

	if (SearchItemComponent)
	{
		SearchItemComponent->OnNewItemFound().AddDynamic(this, &ABaseCharacter::OnNewItemFound);
		SearchItemComponent->OnItemLost().AddDynamic(this, &ABaseCharacter::OnItemLost);
	}
	
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		UEnhancedInputLocalPlayerSubsystem* EnhancedInputComponent = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		if (EnhancedInputComponent)
		{
			EnhancedInputComponent->AddMappingContext(MappingContext, 0);
		}

		PC = PlayerController;
	}

	CharacterReadyDelegate.Broadcast(this);
}

void ABaseCharacter::UpdateSprintStatus() const
{
	const float CurrentSpeed = GetCharacterMovement()->Velocity.Length();
	const bool bIsSprinting = (GetCharacterMovement()->MaxWalkSpeed == SprintSpeed);

	if (FMath::IsNearlyZero(CurrentSpeed) && bIsSprinting)
	{
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	}
}

float ABaseCharacter::AbsorbShieldDamage(const float DamageAmount)
{
	if (!ShieldComponent)
	{
		return 0.f;
	}

	if (DamageAmount <= 0.f)
	{
		return 0.f;
	}

	const float AbsorbedDamage = ShieldComponent->AbsorbDamage(DamageAmount);
	if (AbsorbedDamage > 0.f)
	{
		AbsorbShieldDamageDelegate.Broadcast(this, AbsorbedDamage, ShieldComponent->GetCurrentShield());
	}
	
	if (ShieldComponent->IsBroken())
	{
		OnShieldBroken();
	}

	return AbsorbedDamage;
}

float ABaseCharacter::TakeHealthDamage(const float DamageAmount)
{
	if (DamageAmount <= 0.f)
	{
		return 0.f;
	}
	
	const float TakenDamage = HealthComponent->TakeDamage(DamageAmount);
	TakeHealthDamageDelegate.Broadcast(this, TakenDamage, HealthComponent->GetCurrentHealth());
	
	if (!HealthComponent->IsAlive())
	{
		OnDeath();
	}

	return TakenDamage;
}

void ABaseCharacter::OnDeath()
{
	// If player restart level, destroy otherwise
	if (PC)
	{
		PC->RestartLevel();
	}
	else
	{
		Destroy();
	}
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateSprintStatus();
}

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float ABaseCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (!HealthComponent->IsAlive())
	{
		return 0.f;
	}

	const float ShieldAbsorbedDamage = AbsorbShieldDamage(DamageAmount);
	const float RemainingDamage = (DamageAmount - ShieldAbsorbedDamage <= 0.f)? 0.f : DamageAmount - ShieldAbsorbedDamage;
	
	return TakeHealthDamage(RemainingDamage);
}

void ABaseCharacter::RequestMove(const FVector2d& AxisValue)
{
	const FRotator CurrentRotation = GetControlRotation();

	const float ForwardAxisValue = AxisValue.X;
	const float RightAxisValue = AxisValue.Y;

	AddMovementInput(FRotationMatrix{CurrentRotation}.GetScaledAxis(EAxis::X), ForwardAxisValue);
	AddMovementInput(FRotationMatrix{CurrentRotation}.GetScaledAxis(EAxis::Y), RightAxisValue);
}

void ABaseCharacter::RequestLook(const FVector2d& AxisValue)
{
	const float PitchAxisValue = AxisValue.Y;
	const float YawAxisValue = AxisValue.X;

	const float LookUpRate = BaseLookUpRate;
	const float LookRightRate = BaseLookRightRate;
		
	AddControllerPitchInput(PitchAxisValue * LookUpRate * GetWorld()->GetDeltaSeconds());
	AddControllerYawInput(YawAxisValue * LookRightRate * GetWorld()->GetDeltaSeconds());
}

void ABaseCharacter::RequestToggleSprint() const
{
	if (GetCharacterMovement()->Velocity.Length() <= 0.f)
	{
		return;
	}
	
	if (GetCharacterMovement()->MaxWalkSpeed == WalkSpeed)
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	}
}

void ABaseCharacter::RequestJump()
{
	Jump();
}

void ABaseCharacter::RequestStopJumping()
{
	StopJumping();
}

void ABaseCharacter::RequestWeaponPullTrigger() const
{
	if (WeaponInventoryComponent)
	{
		WeaponInventoryComponent->PullTrigger();
	}
}

void ABaseCharacter::RequestWeaponReleaseTrigger() const
{
	if (WeaponInventoryComponent)
	{
		WeaponInventoryComponent->ReleaseTrigger();
	}
}

void ABaseCharacter::RequestReloadCurrentWeapon()
{
	// No Weapon to reload
	if (WeaponInventoryComponent == nullptr)
	{
		return;
	}
	
	ABaseWeapon* CurrentWeapon = WeaponInventoryComponent->GetCurrentWeapon();
	// No Weapon to reload
	if (CurrentWeapon == nullptr)
	{
		return;
	}

	if (CurrentWeapon->GetCurrentAmmo() == CurrentWeapon->GetMagCapacity())
	{
		return;
	}
	
	// TODO If no inventory we have infinite ammo?
	float AmmoToReload = CurrentWeapon->GetMagCapacity();
	if (AmmoInventoryComponent)
	{
		// TODO Configuration -> If true we reload missing ammo, otherwise the entire capacity
		const float MissingAmmo = CurrentWeapon->GetMissingAmmo();
		AmmoToReload = AmmoInventoryComponent->UseAmmo(CurrentWeapon->GetAmmoType(), MissingAmmo);
	}

	if (AmmoToReload <= 0)
	{
		return;
	}
	
	CurrentWeapon->Reload(AmmoToReload);
}

void ABaseCharacter::RequestChangeWeapon(const int32 WeaponIndex) const
{
	if (WeaponInventoryComponent)
	{
		WeaponInventoryComponent->ChangeWeapon(WeaponIndex);
	}
}

void ABaseCharacter::RequestInteract()
{
	if (WeaponInventoryComponent && WeaponFoundRef)
	{
		WeaponInventoryComponent->EquipWeapon(WeaponFoundRef);
	}
}

float ABaseCharacter::GetMaxHealth() const
{
	return HealthComponent->GetMaxHealth();
}

float ABaseCharacter::GetCurrentHealth() const
{
	return HealthComponent->GetCurrentHealth();
}

bool ABaseCharacter::HasShield() const
{
	return ShieldComponent != nullptr;
}

float ABaseCharacter::GetMaxShield() const
{
	if (HasShield())
	{
		return ShieldComponent->GetMaxShield();
	}

	return 0.f;
}

float ABaseCharacter::GetCurrentShield() const
{
	if (HasShield())
	{
		return ShieldComponent->GetCurrentShield();
	}

	return 0.f;
}

bool ABaseCharacter::AddHealthRegen(const float HealthAmount)
{
	if (HealthComponent->RegenHealth(HealthAmount))
	{
		RegenHealthDelegate.Broadcast(this, HealthAmount, HealthComponent->GetCurrentHealth());
		return true;
	}

	return false;
}

bool ABaseCharacter::AddShieldRegen(const float ShieldAmount)
{
	if (!ShieldComponent)
	{
		return false;
	}
	
	if (ShieldComponent->RegenShield(ShieldAmount))
	{
		RegenShieldDelegate.Broadcast(this, ShieldAmount, ShieldComponent->GetCurrentShield());
		return true;
	}

	return false;
}

bool ABaseCharacter::AddAmmo(const EAmmoType AmmoType, const int32 Amount)
{
	if (AmmoInventoryComponent == nullptr)
	{
		return false;
	}

	AmmoInventoryComponent->AddAmmo(AmmoType, Amount);
	return true;
}

bool ABaseCharacter::HasAmmoInventory() const
{
	return (AmmoInventoryComponent != nullptr);
}

int32 ABaseCharacter::GetCurrentWeaponAmmo() const
{
	if (!WeaponInventoryComponent)
	{
		return 0;
	}
	
	if (WeaponInventoryComponent->GetCurrentWeapon())
	{
		return WeaponInventoryComponent->GetCurrentWeapon()->GetCurrentAmmo();
	}

	return 0;
}

int32 ABaseCharacter::GetCurrentWeaponMaxAmmo() const
{
	if (!WeaponInventoryComponent)
	{
		return 0;
	}
	
	if (WeaponInventoryComponent->GetCurrentWeapon())
	{
		return WeaponInventoryComponent->GetCurrentWeapon()->GetMagCapacity();
	}

	return 0;
}

int32 ABaseCharacter::GetCurrentWeaponAmmoStorage() const
{
	if (!AmmoInventoryComponent || !WeaponInventoryComponent)
	{
		return 0;
	}

	if (WeaponInventoryComponent->GetCurrentWeapon())
	{
		return AmmoInventoryComponent->GetAmmoStorage(WeaponInventoryComponent->GetCurrentWeapon()->GetAmmoType());
	}
	
	return 0;
}

EAmmoType ABaseCharacter::GetCurrentWeaponAmmoType() const
{
	if (WeaponInventoryComponent->GetCurrentWeapon())
	{
		return WeaponInventoryComponent->GetCurrentWeapon()->GetAmmoType();
	}

	return EAmmoType::Default;
}

void ABaseCharacter::OnNewItemFound(const FHitResult& HitResult, AActor* ItemFound)
{
	ABaseWeapon* NewWeapon = Cast<ABaseWeapon>(ItemFound);
	if (NewWeapon)
	{
		if (WeaponFoundRef)
		{
			WeaponFoundRef->DisableHighlight();
		}

		WeaponFoundRef = NewWeapon;
		WeaponFoundRef->EnableHighlight();
	}
}

void ABaseCharacter::OnItemLost(AActor* ItemLost)
{
	if (WeaponFoundRef)
	{
		WeaponFoundRef->DisableHighlight();
		WeaponFoundRef = nullptr;
	}
}

void ABaseCharacter::OnShieldBroken()
{
	
}
