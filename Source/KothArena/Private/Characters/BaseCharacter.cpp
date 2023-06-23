// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BaseCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/AmmoInventoryComponent.h"
#include "Components/HealthComponent.h"
#include "Components/HighlightComponent.h"
#include "Components/SearchItemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/ShieldComponent.h"
#include "Components/ThrowComponent.h"
#include "Components/WeaponInventoryComponent.h"
#include "Gameplay/Throwable/BaseThrowable.h"
#include "Gameplay/Items/ThrowableItem.h"
#include "Net/UnrealNetwork.h"
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

	bReplicates = true;
	SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ABaseCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	InitializeCharacter();
}

// This one is called only on server
void ABaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (APlayerController* PlayerController = Cast<APlayerController>(NewController))
	{
		PC = PlayerController;
	}

	RequestEquipDefaultWeapon();
}

void ABaseCharacter::InitializeCharacter()
{
	ShieldComponent = FindComponentByClass<UShieldComponent>();
	WeaponInventoryComponent = FindComponentByClass<UWeaponInventoryComponent>();
	AmmoInventoryComponent = FindComponentByClass<UAmmoInventoryComponent>();
	SearchItemComponent = FindComponentByClass<USearchItemComponent>();
	ThrowComponent = FindComponentByClass<UThrowComponent>();

	if (SearchItemComponent)
	{
		SearchItemComponent->OnNewItemFound().AddDynamic(this, &ABaseCharacter::OnNewItemFound);
		SearchItemComponent->OnItemLost().AddDynamic(this, &ABaseCharacter::OnItemLost);
	}

	if (ThrowComponent)
	{
		ThrowComponent->OnChangeThrowable().AddDynamic(this, &ABaseCharacter::OnChangeThrowable);
		ThrowComponent->OnChangeQuantity().AddDynamic(this, &ABaseCharacter::OnChangeThrowableQuantity);
	}

	WalkSpeed = (GetCharacterMovement())? GetCharacterMovement()->MaxWalkSpeed : WalkSpeed;
	IdleFov = (CameraComponent)? CameraComponent->FieldOfView : IdleFov;
	CharacterReadyDelegate.Broadcast(this);
}

// You should also override UnPossesed()

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

	if (DamageAmount <= 0.f || ShieldComponent->IsBroken())
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
	CharacterDeathDelegate.Broadcast(this, LastDamageCauserController);
	if (PC)
	{
		PC->RestartLevel();
	}
	else
	{
		Destroy();
	}
}

void ABaseCharacter::RequestEquipDefaultWeapon()
{
	if (!HasAuthority())
	{
		return;
	}
	
	if (bDefaultWeaponSpawned)
	{
		return;
	}
	
	if (WeaponInventoryComponent && WeaponInventoryComponent->ShouldSpawnDefaultWeaponOnBeginPlay())
	{
		bDefaultWeaponSpawned = true;
		WeaponInventoryComponent->EquipDefaultWeapon();
	}
}

void ABaseCharacter::RequestEquipWeapon(ABaseWeapon* NewWeapon)
{
	if (WeaponInventoryComponent)
	{
		WeaponInventoryComponent->EquipWeapon(NewWeapon);
		ServerRequestEquipWeapon(NewWeapon);
	}
}

void ABaseCharacter::ServerRequestEquipWeapon_Implementation(ABaseWeapon* NewWeapon)
{
	MulticastRequestEquipWeapon(NewWeapon);
}

void ABaseCharacter::MulticastRequestEquipWeapon_Implementation(ABaseWeapon* NewWeapon)
{
	if (WeaponInventoryComponent)
	{
		WeaponInventoryComponent->EquipWeapon(NewWeapon);
	}
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateSprintStatus();
	UpdateAim(DeltaTime);
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

	if (!HasAuthority())
	{
		return 0.f;
	}
	
	if (!HealthComponent->IsAlive())
	{
		return 0.f;
	}

	LastDamageCauserController = EventInstigator;
	TakeDamageDelegate.Broadcast(GetController(), LastDamageCauserController);
	const float ShieldAbsorbedDamage = AbsorbShieldDamage(DamageAmount);
	const float RemainingDamage = (DamageAmount - ShieldAbsorbedDamage <= 0.f)? 0.f : DamageAmount - ShieldAbsorbedDamage;
	
	return TakeHealthDamage(RemainingDamage);
}

void ABaseCharacter::RequestMove(const FVector2d& AxisValue)
{
	const FRotator ControlRotation = GetControlRotation();
	const FRotator CurrentRotation{0.f, ControlRotation.Yaw, 0.f};

	const float ForwardAxisValue = AxisValue.X;
	const float RightAxisValue = AxisValue.Y;

	AddMovementInput(FRotationMatrix{CurrentRotation}.GetScaledAxis(EAxis::X), ForwardAxisValue);
	AddMovementInput(FRotationMatrix{CurrentRotation}.GetScaledAxis(EAxis::Y), RightAxisValue);
}

void ABaseCharacter::RequestLook(const FVector2d& AxisValue)
{
	const float PitchAxisValue = AxisValue.Y;
	const float YawAxisValue = AxisValue.X;

	const float LookUpRate = GetLookUpRate();
	const float LookRightRate = GetLookRightRate();
		
	AddControllerPitchInput(PitchAxisValue * LookUpRate * GetWorld()->GetDeltaSeconds());
	AddControllerYawInput(YawAxisValue * LookRightRate * GetWorld()->GetDeltaSeconds());
}

void ABaseCharacter::RequestToggleSprint()
{
	if (!HasAuthority())
	{
		// Sprint just on client
		HandleToggleSprint();
	}
	
	ServerRequestSprintToggle();
}

void ABaseCharacter::HandleToggleSprint()
{
	if (GetCharacterMovement()->Velocity.Length() <= 0.f)
	{
		return;
	}
	
	if (GetCharacterMovement()->MaxWalkSpeed == WalkSpeed)
	{
		HandleRequestEndAiming();
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	}
}

void ABaseCharacter::HandleRequestStartAiming()
{
	if (CombatState != ECharacterCombatState::Reloading)
	{
		CombatState = ECharacterCombatState::Aiming;
		bUseControllerRotationYaw = true;
		GetCharacterMovement()->MaxWalkSpeed = AimingWalkSpeed;
	}
}

void ABaseCharacter::HandleRequestEndAiming()
{
	if (CombatState == ECharacterCombatState::Aiming)
	{
		CombatState = ECharacterCombatState::Idle;
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	}
}

void ABaseCharacter::HandleRequestReload()
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

	HandleRequestEndAiming();

	CombatState = ECharacterCombatState::Reloading;
	
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
		CombatState = ECharacterCombatState::Idle;
		return;
	}
	
	CurrentWeapon->Reload(AmmoToReload);
	CombatState = ECharacterCombatState::Idle;
}

void ABaseCharacter::UpdateAim(const float DeltaTime)
{
	if (!CameraComponent)
	{
		return;	
	}

	if (IsAiming())
	{
		CurrentTimeAiming = FMath::Clamp(CurrentTimeAiming + DeltaTime, 0.f, TimeToAim);
	}
	else
	{
		CurrentTimeAiming = FMath::Clamp(CurrentTimeAiming - DeltaTime, 0.f, TimeToAim);
	}

	const float TimeRatio = FMath::Clamp(CurrentTimeAiming / TimeToAim, 0.f, 1.f);
	const float NewFov = FMath::Lerp(IdleFov, AimingFov, TimeRatio);
	CameraComponent->FieldOfView = NewFov;
}

void ABaseCharacter::HandleRequestChangeThrowable(const AThrowableItem* NewThrowableClass, const int32 Quantity)
{
	if (ThrowComponent && NewThrowableClass)
	{
		ThrowComponent->ChangeThrowable(NewThrowableClass->GetThrowableClass(), Quantity);
	}
}

void ABaseCharacter::HandleRequestAddThrowableQuantity(const int32 Quantity)
{
	if (ThrowComponent)
	{
		ThrowComponent->AddQuantity(Quantity);
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
	HandleRequestReload();
	ServerRequestReload();
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
	if (WeaponFoundRef)
	{
		RequestEquipWeapon(WeaponFoundRef);
	}
	else if (ThrowableItemFoundRef)
	{
		ServerRequestChangeThrowable(ThrowableItemFoundRef, ThrowableItemFoundRef->GetQuantity());
	}
}

void ABaseCharacter::RequestStartAiming()
{
	HandleRequestStartAiming();
	ServerRequestStartAiming();
}

void ABaseCharacter::RequestEndAiming()
{
	HandleRequestEndAiming();
	ServerRequestEndAiming();
}

void ABaseCharacter::RequestStartThrowing()
{
	ServerRequestStartThrowing();
}

void ABaseCharacter::RequestFinishThrowing()
{
	ServerRequestFinishThrowing();
}

void ABaseCharacter::RequestAddThrowableQuantity(const int32 Quantity)
{
	if (!HasAuthority())
	{
		return;
	}

	HandleRequestAddThrowableQuantity(Quantity);
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
	if (!HasAuthority())
	{
		return false;
	}
	
	if (HealthComponent->RegenHealth(HealthAmount))
	{
		RegenHealthDelegate.Broadcast(this, HealthAmount, HealthComponent->GetCurrentHealth());
		return true;
	}

	return false;
}

bool ABaseCharacter::AddShieldRegen(const float ShieldAmount)
{
	if (!HasAuthority())
	{
		return false;
	}
	
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

ABaseWeapon* ABaseCharacter::GetCurrentWeapon() const
{
	if (WeaponInventoryComponent)
	{
		return WeaponInventoryComponent->GetCurrentWeapon();
	}

	return nullptr;
}

float ABaseCharacter::GetLookUpRate() const
{
	return (IsAiming())? AimingLookUpRate : BaseLookUpRate;
}

float ABaseCharacter::GetLookRightRate() const
{
	return (IsAiming())? AimingLookRightRate : BaseLookRightRate;
}

TSubclassOf<ABaseThrowable> ABaseCharacter::GetCurrentThrowable() const
{
	if (ThrowComponent)
	{
		return ThrowComponent->GetCurrentThrowable();
	}

	return nullptr;
}

void ABaseCharacter::NotifyShieldDamage(const float DamageAbsorbed, const float NewShield)
{
	ensure(IsLocallyControlled());
	AbsorbShieldDamageDelegate.Broadcast(this, DamageAbsorbed, NewShield);
}

void ABaseCharacter::NotifyShieldRegen(const float Amount, const float NewShield)
{
	ensure(IsLocallyControlled());
	RegenShieldDelegate.Broadcast(this, Amount, NewShield);
}

void ABaseCharacter::NotifyHealthDamage(const float DamageAbsorbed, const float NewHealth)
{
	ensure(IsLocallyControlled());
	TakeHealthDamageDelegate.Broadcast(this, DamageAbsorbed, NewHealth);
}

void ABaseCharacter::NotifyHealthRegen(const float Amount, const float NewHealth)
{
	ensure(IsLocallyControlled());
	RegenHealthDelegate.Broadcast(this, Amount, NewHealth);
}

void ABaseCharacter::OnNewItemFound(const FHitResult& HitResult, AActor* ItemFound)
{
	// If there's a new item
	if (ItemFound != nullptr)
	{
		// Disable highlight on older item
		if (ItemFoundRef != nullptr)
		{
			if (UHighlightComponent* OldHighlight = ItemFoundRef->FindComponentByClass<UHighlightComponent>())
			{
				OldHighlight->DisableHighlight();
			}
		}

		// Highlight new item found
		if (UHighlightComponent* NewHighlightComponent = ItemFound->FindComponentByClass<UHighlightComponent>())
		{
			NewHighlightComponent->EnableHighlight();
			ItemFoundRef = ItemFound;
		}
	}

	if (ABaseWeapon* NewWeapon = Cast<ABaseWeapon>(ItemFound))
	{
		if (NewWeapon == GetCurrentWeapon())
		{
			return;
		}
		
		WeaponFoundRef = NewWeapon;
	}
	else if (AThrowableItem* NewThrowable = Cast<AThrowableItem>(ItemFound))
	{
		ThrowableItemFoundRef = NewThrowable;	
	}
}

void ABaseCharacter::OnItemLost(AActor* ItemLost)
{
	if (ItemFoundRef != nullptr)
	{
		// Disable highlight on older item
		if (UHighlightComponent* OldHighlight = ItemFoundRef->FindComponentByClass<UHighlightComponent>())
		{
			OldHighlight->DisableHighlight();
		}
		
		ItemFoundRef = nullptr;
		WeaponFoundRef = nullptr;
		ThrowableItemFoundRef = nullptr;
	}
}

void ABaseCharacter::OnChangeThrowable(TSubclassOf<ABaseThrowable> NewThrowableClass, const int32 Quantity)
{
	CharacterChangeThrowableDelegate.Broadcast(NewThrowableClass, Quantity);
}

void ABaseCharacter::OnChangeThrowableQuantity(const int32 NewQuantity)
{
	CharacterChangeThrowableQuantityDelegate.Broadcast(NewQuantity);
}

void ABaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseCharacter, bDefaultWeaponSpawned);
	DOREPLIFETIME(ABaseCharacter, CombatState);
}

void ABaseCharacter::ServerRequestAddThrowableQuantity_Implementation(const int32 Quantity)
{
	HandleRequestAddThrowableQuantity(Quantity);
}

void ABaseCharacter::ServerRequestChangeThrowable_Implementation(AThrowableItem* NewThrowableClass,
	const int32 Quantity)
{
	HandleRequestChangeThrowable(NewThrowableClass, Quantity);
	NewThrowableClass->Destroy();
}

void ABaseCharacter::ServerRequestStartThrowing_Implementation()
{
	if (ThrowComponent)
	{
		CombatState = ECharacterCombatState::Throwing;
		ThrowComponent->StartThrowing();
	}
}

void ABaseCharacter::ServerRequestFinishThrowing_Implementation()
{
	if (ThrowComponent)
	{
		CombatState = ECharacterCombatState::Idle;
		ThrowComponent->FinishThrowing();
	}
}

void ABaseCharacter::ServerRequestReload_Implementation()
{
	HandleRequestReload();
}

void ABaseCharacter::ServerRequestStartAiming_Implementation()
{
	HandleRequestStartAiming();
}

void ABaseCharacter::ServerRequestEndAiming_Implementation()
{
	HandleRequestEndAiming();
}

void ABaseCharacter::ServerRequestSprintToggle_Implementation()
{
	HandleToggleSprint();
}

void ABaseCharacter::OnShieldBroken()
{
	CharacterShieldBrokenDelegate.Broadcast(this, LastDamageCauserController);
}
