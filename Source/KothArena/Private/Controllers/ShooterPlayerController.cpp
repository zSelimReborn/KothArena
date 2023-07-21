// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/ShooterPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "UI/PlayerHud.h"
#include "Characters/BaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Gameplay/Throwable/BaseThrowable.h"

void AShooterPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void AShooterPlayerController::AcknowledgePossession(APawn* P)
{
	Super::AcknowledgePossession(P);
	SetupCharacter(P);
}

void AShooterPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (InputComponent)
	{
		UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
		if (EnhancedInputComponent)
		{
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AShooterPlayerController::RequestJumpAction);
			EnhancedInputComponent->BindAction(StopJumpAction, ETriggerEvent::Triggered, this, &AShooterPlayerController::RequestStopJumpAction);
			
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AShooterPlayerController::RequestMoveAction);
			
			EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AShooterPlayerController::RequestLookAction);

			EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &AShooterPlayerController::RequestSprintAction);

			EnhancedInputComponent->BindAction(PullTriggerAction, ETriggerEvent::Started, this, &AShooterPlayerController::RequestWeaponPullTriggerAction);
			EnhancedInputComponent->BindAction(PullTriggerAction, ETriggerEvent::Completed, this, &AShooterPlayerController::RequestWeaponReleaseTriggerAction);

			EnhancedInputComponent->BindAction(ReloadWeaponAction, ETriggerEvent::Triggered, this, &AShooterPlayerController::RequestReloadWeapon);
			EnhancedInputComponent->BindAction(ChangeWeaponFirstSlotAction, ETriggerEvent::Triggered, this, &AShooterPlayerController::RequestChangeWeaponFirstSlot);
			EnhancedInputComponent->BindAction(ChangeWeaponSecondSlotAction, ETriggerEvent::Triggered, this, &AShooterPlayerController::RequestChangeWeaponSecondSlot);
			EnhancedInputComponent->BindAction(ChangeWeaponThirdSlotAction, ETriggerEvent::Triggered, this, &AShooterPlayerController::RequestChangeWeaponThirdSlot);
			EnhancedInputComponent->BindAction(ChangeWeaponFourthSlotAction, ETriggerEvent::Triggered, this, &AShooterPlayerController::RequestChangeWeaponFourthSlot);

			EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &AShooterPlayerController::RequestInteract);

			EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &AShooterPlayerController::RequestStartAiming);
			EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &AShooterPlayerController::RequestEndAiming);

			EnhancedInputComponent->BindAction(ThrowAction, ETriggerEvent::Started, this, &AShooterPlayerController::RequestStartThrowing);
			EnhancedInputComponent->BindAction(ThrowAction, ETriggerEvent::Completed, this, &AShooterPlayerController::RequestFinishThrowing);

			InitializeMappingContext();
		}
	}
}

bool AShooterPlayerController::HasAmmoInventory() const
{
	return (BaseCharacterRef)? BaseCharacterRef->HasAmmoInventory() : false;
}

int32 AShooterPlayerController::GetCurrentWeaponAmmo() const
{
	return (BaseCharacterRef)? BaseCharacterRef->GetCurrentWeaponAmmo() : 0;
}

int32 AShooterPlayerController::GetCurrentWeaponMaxAmmo() const
{
	return (BaseCharacterRef)? BaseCharacterRef->GetCurrentWeaponMaxAmmo() : 0;
}

int32 AShooterPlayerController::GetCurrentWeaponAmmoStorage() const
{
	return (BaseCharacterRef)? BaseCharacterRef->GetCurrentWeaponAmmoStorage() : 0;
}

EAmmoType AShooterPlayerController::GetCurrentWeaponAmmoType() const
{
	return (BaseCharacterRef)? BaseCharacterRef->GetCurrentWeaponAmmoType() : EAmmoType::Default;
}

void AShooterPlayerController::OnCharacterReady(ACharacter* InstigatorCharacter)
{
	InitializeHud();
	InitializeHudDelegates();
	FinalizeInitialize();
}

void AShooterPlayerController::OnCharacterAbsorbShieldDamage(ACharacter* InstigatorCharacter,
	const float AbsorbedDamage, const float NewShieldValue)
{
	if (PlayerHudRef)
	{
		PlayerHudRef->OnAbsorbShieldDamage(AbsorbedDamage, NewShieldValue);
	}
}

void AShooterPlayerController::OnCharacterTakeHealthDamage(ACharacter* InstigatorCharacter, const float TakenDamage,
	const float NewHealthValue)
{
	if (PlayerHudRef)
	{
		PlayerHudRef->OnTakeHealthDamage(TakenDamage, NewHealthValue);
	}
}

void AShooterPlayerController::OnCharacterRegenShield(ACharacter* InstigatorCharacter, const float RegenAmount,
	const float NewShieldValue)
{
	if (PlayerHudRef)
	{
		PlayerHudRef->OnRegenShield(RegenAmount, NewShieldValue);
	}
}

void AShooterPlayerController::OnCharacterRegenHealth(ACharacter* InstigatorCharacter, const float RegenAmount,
	const float NewHealthValue)
{
	if (PlayerHudRef)
	{
		PlayerHudRef->OnRegenHealth(RegenAmount, NewHealthValue);
	}
}

void AShooterPlayerController::OnCharacterChangeThrowable(
	TSubclassOf<ABaseThrowable> NewThrowableClass,
	const int32 Quantity
) {
	if (PlayerHudRef)
	{
		const ABaseThrowable* ThrowableDefault = NewThrowableClass->GetDefaultObject<ABaseThrowable>();
		if (ThrowableDefault)
		{
			PlayerHudRef->OnChangeThrowable(ThrowableDefault->GetThrowableName(), ThrowableDefault->GetThrowableThumbnail(), Quantity);
		}
	}
}

void AShooterPlayerController::OnCharacterChangeThrowableQuantity(const int32 NewQuantity)
{
	if (PlayerHudRef)
	{
		PlayerHudRef->OnChangeThrowableQuantity(NewQuantity);
	}
}

void AShooterPlayerController::OnCharacterEquipWeapon(ABaseWeapon* EquippedWeapon)
{
	if (PlayerHudRef)
	{
		PlayerHudRef->OnEquipWeapon(EquippedWeapon);
	}
}

void AShooterPlayerController::OnCharacterChangeWeapon(ABaseWeapon* CurrentWeapon)
{
	if (PlayerHudRef)
	{
		PlayerHudRef->OnChangeWeapon(CurrentWeapon);
	}
}

void AShooterPlayerController::InitializeMappingContext()
{
	UEnhancedInputLocalPlayerSubsystem* EnhancedInputLocalPlayerSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (EnhancedInputLocalPlayerSubsystem)
	{
		EnhancedInputLocalPlayerSubsystem->AddMappingContext(MappingContext, 0);
	}
}

void AShooterPlayerController::SetupCharacter(AActor* NewCharacter)
{
	BaseCharacterRef = Cast<ABaseCharacter>(NewCharacter);
	if (BaseCharacterRef != nullptr)
	{
		OnCharacterReady(BaseCharacterRef);
		if (!BaseCharacterRef->OnCharacterReady().IsAlreadyBound(this, &AShooterPlayerController::OnCharacterReady))
		{
			BaseCharacterRef->OnCharacterReady().AddDynamic(this, &AShooterPlayerController::OnCharacterReady);
		}

		ServerPrepareForBattle();
	}
}

void AShooterPlayerController::ClientOnCharacterHitSomeone_Implementation()
{
	if (PlayerHudRef)
	{
		PlayerHudRef->OnHitSomeone();
	}
}

void AShooterPlayerController::ClientOnCharacterBrokeShield_Implementation()
{
	if (PlayerHudRef)
	{
		PlayerHudRef->OnBrokeShield();
	}
}

void AShooterPlayerController::ClientOnCharacterKillSomeone_Implementation(const int32 NewKillCount)
{
	KillCount = NewKillCount;

	if (PlayerHudRef)
	{
		PlayerHudRef->OnKillSomeone(KillCount);
	}
}

void AShooterPlayerController::ServerPrepareForBattle_Implementation()
{
	BaseCharacterRef = Cast<ABaseCharacter>(GetCharacter());
	if (BaseCharacterRef != nullptr)
	{
		BaseCharacterRef->PrepareForBattle();
	}
}

void AShooterPlayerController::OnCharacterHitSomeone()
{
	ClientOnCharacterHitSomeone();
}

void AShooterPlayerController::OnCharacterKillSomeone(const int32 NewKillCount)
{
	ClientOnCharacterKillSomeone(NewKillCount);
}

void AShooterPlayerController::OnCharacterBrokeShield()
{
	ClientOnCharacterBrokeShield();
}

void AShooterPlayerController::InitializeHud()
{
	if (PlayerHudClass && IsLocalController())
	{
		if (PlayerHudRef == nullptr)
		{
			PlayerHudRef = CreateWidget<UPlayerHud>(this, PlayerHudClass);
		}
		
		if (BaseCharacterRef && PlayerHudRef)
		{
			if (!PlayerHudRef->IsInViewport())
			{
				PlayerHudRef->AddToPlayerScreen();
			}
			
			PlayerHudRef->InitializeOwnerController(this);
			PlayerHudRef->InitializeHealthAndShield(
				true,
				BaseCharacterRef->GetMaxHealth(),
				BaseCharacterRef->GetCurrentHealth(),
				BaseCharacterRef->HasShield(),
				BaseCharacterRef->GetMaxShield(),
				BaseCharacterRef->GetCurrentShield()
			);
		}
	}
}

void AShooterPlayerController::InitializeHudDelegates()
{
	if (BaseCharacterRef && IsLocalController())
	{
		if (!BaseCharacterRef->OnAbsorbShieldDamage().IsAlreadyBound(this, &AShooterPlayerController::OnCharacterAbsorbShieldDamage))
		{
			BaseCharacterRef->OnAbsorbShieldDamage().AddDynamic(this, &AShooterPlayerController::OnCharacterAbsorbShieldDamage);
		}
		if (!BaseCharacterRef->OnTakeHealthDamage().IsAlreadyBound(this, &AShooterPlayerController::OnCharacterTakeHealthDamage))
		{
			BaseCharacterRef->OnTakeHealthDamage().AddDynamic(this, &AShooterPlayerController::OnCharacterTakeHealthDamage);
		}
		if (!BaseCharacterRef->OnRegenShield().IsAlreadyBound(this, &AShooterPlayerController::OnCharacterRegenShield))
		{
			BaseCharacterRef->OnRegenShield().AddDynamic(this, &AShooterPlayerController::OnCharacterRegenShield);
		}
		if (!BaseCharacterRef->OnRegenHealth().IsAlreadyBound(this, &AShooterPlayerController::OnCharacterRegenHealth))
		{
			BaseCharacterRef->OnRegenHealth().AddDynamic(this, &AShooterPlayerController::OnCharacterRegenHealth);
		}
		if (!BaseCharacterRef->OnCharacterChangeThrowable().IsAlreadyBound(this, &AShooterPlayerController::OnCharacterChangeThrowable))
		{
			BaseCharacterRef->OnCharacterChangeThrowable().AddDynamic(this, &AShooterPlayerController::OnCharacterChangeThrowable);
		}
		if (!BaseCharacterRef->OnCharacterChangeThrowableQuantity().IsAlreadyBound(this, &AShooterPlayerController::OnCharacterChangeThrowableQuantity))
		{
			BaseCharacterRef->OnCharacterChangeThrowableQuantity().AddDynamic(this, &AShooterPlayerController::OnCharacterChangeThrowableQuantity);
		}
		if (!BaseCharacterRef->OnCharacterEquipWeapon().IsAlreadyBound(this, &AShooterPlayerController::OnCharacterEquipWeapon))
		{
			BaseCharacterRef->OnCharacterEquipWeapon().AddDynamic(this, &AShooterPlayerController::OnCharacterEquipWeapon);
		}
		if (!BaseCharacterRef->OnCharacterChangeWeapon().IsAlreadyBound(this, &AShooterPlayerController::OnCharacterChangeWeapon))
		{
			BaseCharacterRef->OnCharacterChangeWeapon().AddDynamic(this, &AShooterPlayerController::OnCharacterChangeWeapon);
		}
	}
}

void AShooterPlayerController::FinalizeInitialize() const {}

void AShooterPlayerController::RequestJumpAction()
{
	if (BaseCharacterRef)
	{
		BaseCharacterRef->RequestJump();
	}
}

void AShooterPlayerController::RequestStopJumpAction()
{
	if (BaseCharacterRef)
	{
		BaseCharacterRef->StopJumping();
	}
}

void AShooterPlayerController::RequestMoveAction(const FInputActionValue& Value)
{
	const FVector2d MoveValue = Value.Get<FVector2d>();
	if (BaseCharacterRef)
	{
		BaseCharacterRef->RequestMove(Value.Get<FVector2d>());
	}

	if (PlayerHudRef)
	{
		PlayerHudRef->OnPlayerMove(MoveValue.X, MoveValue.Y);
	}
}

void AShooterPlayerController::RequestLookAction(const FInputActionValue& Value)
{
	const FVector2d CameraMoveValue = Value.Get<FVector2d>();
	if (BaseCharacterRef)
	{
		BaseCharacterRef->RequestLook(CameraMoveValue);
	}

	if (PlayerHudRef)
	{
		PlayerHudRef->OnCameraMove(CameraMoveValue.X, CameraMoveValue.Y);
	}
}

void AShooterPlayerController::RequestSprintAction()
{
	if (BaseCharacterRef)
	{
		BaseCharacterRef->RequestToggleSprint();
	}
}

void AShooterPlayerController::RequestWeaponPullTriggerAction()
{
	if (BaseCharacterRef)
	{
		BaseCharacterRef->RequestWeaponPullTrigger();
	}
}

void AShooterPlayerController::RequestWeaponReleaseTriggerAction()
{
	if (BaseCharacterRef)
	{
		BaseCharacterRef->RequestWeaponReleaseTrigger();
	}
}

void AShooterPlayerController::RequestReloadWeapon()
{
	if (BaseCharacterRef)
	{
		BaseCharacterRef->RequestReloadCurrentWeapon();
	}
}

void AShooterPlayerController::RequestInteract()
{
	if (BaseCharacterRef)
	{
		BaseCharacterRef->RequestInteract();
	}
}

void AShooterPlayerController::RequestChangeWeaponFirstSlot()
{
	RequestChangeWeapon(0);
}

void AShooterPlayerController::RequestChangeWeaponSecondSlot()
{
	RequestChangeWeapon(1);
}

void AShooterPlayerController::RequestChangeWeaponThirdSlot()
{
	RequestChangeWeapon(2);
}

void AShooterPlayerController::RequestChangeWeaponFourthSlot()
{
	RequestChangeWeapon(3);
}

void AShooterPlayerController::RequestChangeWeapon(const int32 Index)
{
	if (BaseCharacterRef)
	{
		BaseCharacterRef->RequestChangeWeapon(Index);
	}
}

void AShooterPlayerController::RequestStartAiming()
{
	if (BaseCharacterRef)
	{
		BaseCharacterRef->RequestStartAiming();
	}
}

void AShooterPlayerController::RequestEndAiming()
{
	if (BaseCharacterRef)
	{
		BaseCharacterRef->RequestEndAiming();
	}
}

void AShooterPlayerController::RequestStartThrowing()
{
	if (BaseCharacterRef)
	{
		BaseCharacterRef->RequestStartThrowing();
	}
}

void AShooterPlayerController::RequestFinishThrowing()
{
	if (BaseCharacterRef)
	{
		BaseCharacterRef->RequestFinishThrowing();
	}
}
