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
	ShieldComponent = CreateDefaultSubobject<UShieldComponent>(TEXT("Shield Component"));
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		UEnhancedInputLocalPlayerSubsystem* EnhancedInputComponent = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		if (EnhancedInputComponent)
		{
			EnhancedInputComponent->AddMappingContext(MappingContext, 0);
		}

		PC = PlayerController;
		InitializeHud();
	}
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
	if (PlayerHudRef && AbsorbedDamage > 0.f)
	{
		PlayerHudRef->OnAbsorbShieldDamage(AbsorbedDamage, ShieldComponent->GetCurrentShield());	
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
	if (PlayerHudRef)
	{
		PlayerHudRef->OnTakeHealthDamage(TakenDamage, HealthComponent->GetCurrentHealth());
	}
	
	if (!HealthComponent->IsAlive())
	{
		OnDeath();
	}

	return TakenDamage;
}

void ABaseCharacter::OnDeath()
{
}

void ABaseCharacter::InitializeHud()
{
	if (PlayerHudClass)
	{
		PlayerHudRef = CreateWidget<UPlayerHud>(PC, PlayerHudClass);
		PlayerHudRef->AddToViewport();
		PlayerHudRef->InitializeHealthAndShield(
			(HealthComponent != nullptr),
			(HealthComponent)? HealthComponent->GetMaxHealth() : 0.f,
			(HealthComponent)? HealthComponent->GetCurrentHealth() : 0.f,
			(ShieldComponent != nullptr),
			(ShieldComponent)? ShieldComponent->GetMaxShield() : 0.f,
			(ShieldComponent)? ShieldComponent->GetCurrentShield() : 0.f
		);
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

float ABaseCharacter::GetMaxHealth() const
{
	return HealthComponent->GetMaxHealth();
}

float ABaseCharacter::GetCurrentHealth() const
{
	return HealthComponent->GetCurrentHealth();
}

float ABaseCharacter::GetMaxShield() const
{
	check(ShieldComponent);
	return ShieldComponent->GetMaxShield();
}

float ABaseCharacter::GetCurrentShield() const
{
	check(ShieldComponent);
	return ShieldComponent->GetCurrentShield();
}

bool ABaseCharacter::AddHealthRegen(const float HealthAmount)
{
	if (HealthComponent->RegenHealth(HealthAmount))
	{
		if (PlayerHudRef)
		{
			PlayerHudRef->OnRegenHealth(HealthAmount, HealthComponent->GetCurrentHealth());
		}
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
		if (PlayerHudRef)
		{
			PlayerHudRef->OnRegenShield(ShieldAmount, ShieldComponent->GetCurrentShield());
		}

		return true;
	}

	return false;
}

void ABaseCharacter::OnShieldBroken()
{
	
}
