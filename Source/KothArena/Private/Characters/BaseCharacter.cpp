// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BaseCharacter.h"

#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Components/HealthComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

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

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		UEnhancedInputLocalPlayerSubsystem* EnhancedInputComponent = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
		if (EnhancedInputComponent)
		{
			EnhancedInputComponent->AddMappingContext(MappingContext, 0);
		}
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

	float DamageAbsorbed = 0.f;
	if (HealthComponent->IsAlive())
	{
		DamageAbsorbed = ((HealthComponent->GetCurrentHealth() - DamageAmount) < 0.f)? HealthComponent->GetCurrentHealth() : DamageAmount;
		HealthComponent->TakeDamage(DamageAmount);
		if (!HealthComponent->IsAlive())
		{
			OnDeath();
		}
	}
	
	return DamageAbsorbed;
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

