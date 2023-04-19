// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/ShooterPlayerController.h"

#include "EnhancedInputComponent.h"
#include "Blueprint/UserWidget.h"
#include "UI/PlayerHud.h"
#include "Characters/BaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void AShooterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	BaseCharacterRef = Cast<ABaseCharacter>(GetCharacter());
	if (BaseCharacterRef)
	{
		BaseCharacterRef->OnCharacterReady().AddDynamic(this, &AShooterPlayerController::OnCharacterReady);
	}
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

			EnhancedInputComponent->BindAction(PullTriggerAction, ETriggerEvent::Triggered, this, &AShooterPlayerController::RequestWeaponPullTriggerAction);
			EnhancedInputComponent->BindAction(ReleaseTriggerAction, ETriggerEvent::Triggered, this, &AShooterPlayerController::RequestWeaponReleaseTriggerAction);

			EnhancedInputComponent->BindAction(ReloadWeaponAction, ETriggerEvent::Triggered, this, &AShooterPlayerController::RequestReloadWeapon);
			EnhancedInputComponent->BindAction(ChangeWeaponFirstSlotAction, ETriggerEvent::Triggered, this, &AShooterPlayerController::RequestChangeWeaponFirstSlot);
			EnhancedInputComponent->BindAction(ChangeWeaponSecondSlotAction, ETriggerEvent::Triggered, this, &AShooterPlayerController::RequestChangeWeaponSecondSlot);
			EnhancedInputComponent->BindAction(ChangeWeaponThirdSlotAction, ETriggerEvent::Triggered, this, &AShooterPlayerController::RequestChangeWeaponThirdSlot);
			EnhancedInputComponent->BindAction(ChangeWeaponFourthSlotAction, ETriggerEvent::Triggered, this, &AShooterPlayerController::RequestChangeWeaponFourthSlot);

			EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &AShooterPlayerController::RequestInteract);
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

void AShooterPlayerController::OnCharacterHitSomeone()
{
	if (PlayerHudRef)
	{
		PlayerHudRef->OnHitSomeone();
	}
}

void AShooterPlayerController::OnCharacterKillSomeone(const int32 NewKillCount)
{
	KillCount = NewKillCount;

	if (PlayerHudRef)
	{
		PlayerHudRef->OnKillSomeone(KillCount);
	}
}

void AShooterPlayerController::OnCharacterBrokeShield()
{
	if (PlayerHudRef)
	{
		PlayerHudRef->OnBrokeShield();
	}
}

void AShooterPlayerController::InitializeHud()
{
	if (PlayerHudClass)
	{
		PlayerHudRef = CreateWidget<UPlayerHud>(this, PlayerHudClass);
		PlayerHudRef->AddToViewport();
		if (BaseCharacterRef)
		{
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
	if (BaseCharacterRef)
	{
		BaseCharacterRef->OnAbsorbShieldDamage().AddDynamic(this, &AShooterPlayerController::OnCharacterAbsorbShieldDamage);
		BaseCharacterRef->OnTakeHealthDamage().AddDynamic(this, &AShooterPlayerController::OnCharacterTakeHealthDamage);
		BaseCharacterRef->OnRegenShield().AddDynamic(this, &AShooterPlayerController::OnCharacterRegenShield);
		BaseCharacterRef->OnRegenHealth().AddDynamic(this, &AShooterPlayerController::OnCharacterRegenHealth);
	}
}

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
	if (BaseCharacterRef)
	{
		BaseCharacterRef->RequestMove(Value.Get<FVector2d>());
	}
}

void AShooterPlayerController::RequestLookAction(const FInputActionValue& Value)
{
	if (BaseCharacterRef)
	{
		BaseCharacterRef->RequestLook(Value.Get<FVector2d>());
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
