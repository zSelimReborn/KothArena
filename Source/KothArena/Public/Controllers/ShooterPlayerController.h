// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "ShooterPlayerController.generated.h"

class ABaseCharacter;
class UInputAction;

/**
 * 
 */
UCLASS()
class KOTHARENA_API AShooterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

// Callbacks
protected:
	
// Action functions
protected:
	void RequestJumpAction();
	void RequestStopJumpAction();
	void RequestMoveAction(const FInputActionValue&);
	void RequestLookAction(const FInputActionValue&);
	void RequestSprintAction();
	

// Properties
protected:
	UPROPERTY(Transient)
	TObjectPtr<ABaseCharacter> BaseCharacterRef;
	
// Bindings
protected:
	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> JumpAction;
	
	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> StopJumpAction;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> SprintAction;
};
