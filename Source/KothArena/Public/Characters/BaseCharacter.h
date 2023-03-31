// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UHealthComponent;
class UShieldComponent;
class UPlayerHud;

UCLASS()
class KOTHARENA_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseCharacter();

// Features
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void UpdateSprintStatus() const;

	float AbsorbShieldDamage(const float DamageAmount);

	float TakeHealthDamage(const float DamageAmount);
	
	void OnShieldBroken();

	void OnDeath();

	void InitializeHud();
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	void RequestMove(const FVector2d& AxisValue);
	void RequestLook(const FVector2d& AxisValue);
	void RequestToggleSprint() const;
	void RequestJump();
	void RequestStopJumping();

	UFUNCTION(BlueprintPure)
	float GetMaxHealth() const;

	UFUNCTION(BlueprintPure)
	float GetCurrentHealth() const;

	UFUNCTION(BlueprintPure)
	float GetMaxShield() const;

	UFUNCTION(BlueprintPure)
	float GetCurrentShield() const;

	UFUNCTION(BlueprintCallable)
	bool AddHealthRegen(const float HealthAmount);

	UFUNCTION(BlueprintCallable)
	bool AddShieldRegen(const float ShieldAmount);
	
// Components
protected:
	UPROPERTY(VisibleAnywhere, NoClear)
	TObjectPtr<USpringArmComponent> CameraBoom;
	
	UPROPERTY(VisibleAnywhere, NoClear)
	TObjectPtr<UCameraComponent> CameraComponent;

	UPROPERTY(VisibleAnywhere, NoClear)
	TObjectPtr<UHealthComponent> HealthComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UShieldComponent> ShieldComponent;

// Properties
protected:
	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputMappingContext> MappingContext;

	UPROPERTY(EditAnywhere, Category="Look", meta=(ClampMin="0", UIMin="0", ForceUnits="deg/s"))
	float BaseLookUpRate = 90.f;

	UPROPERTY(EditAnywhere, Category="Look", meta=(ClampMin="0", UIMin="0", ForceUnits="deg/s"))
	float BaseLookRightRate = 90.f;

	UPROPERTY(EditAnywhere, Category="Sprint", meta=(ClampMin="0", UIMin="0", ForceUnits="cm/s"))
	float SprintSpeed = 900.f;

	UPROPERTY(Transient)
	float WalkSpeed = 600.f;

	UPROPERTY(Transient)
	TObjectPtr<APlayerController> PC;
	
	UPROPERTY(EditAnywhere, Category="HUD")
	TSubclassOf<UPlayerHud> PlayerHudClass;

	UPROPERTY(Transient)
	TObjectPtr<UPlayerHud> PlayerHudRef;
};
