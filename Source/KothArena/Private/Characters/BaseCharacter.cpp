// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BaseCharacter.h"

#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Components/HealthComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/ShieldComponent.h"
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

void ABaseCharacter::OnShieldBroken()
{
	
}
