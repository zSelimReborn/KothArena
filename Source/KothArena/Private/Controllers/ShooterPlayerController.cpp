// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/ShooterPlayerController.h"

#include "EnhancedInputComponent.h"
#include "Characters/BaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void AShooterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	BaseCharacterRef = Cast<ABaseCharacter>(GetCharacter());
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
		}
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
