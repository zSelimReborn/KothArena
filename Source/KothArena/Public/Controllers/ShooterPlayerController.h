// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "Gameplay/Items/AmmoItem.h"
#include "ShooterPlayerController.generated.h"

class ABaseCharacter;
class UInputAction;
class UPlayerHud;
class AKothArenaGameModeBase;
class UInputMappingContext;

/**
 * 
 */
UCLASS()
class KOTHARENA_API AShooterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void AcknowledgePossession(APawn* P) override;
	virtual void SetupInputComponent() override;

	// This is used by PlayerHUD
	UFUNCTION(BlueprintPure)
	bool HasAmmoInventory() const;

	// This is used by PlayerHUD
	UFUNCTION(BlueprintPure)
	int32 GetCurrentWeaponAmmo() const;

	// This is used by PlayerHUD
	UFUNCTION(BlueprintPure)
	int32 GetCurrentWeaponMaxAmmo() const;

	// This is used by PlayerHUD
	UFUNCTION(BlueprintPure)
	int32 GetCurrentWeaponAmmoStorage() const;

	// This is used by PlayerHUD
	UFUNCTION(BlueprintPure)
	EAmmoType GetCurrentWeaponAmmoType() const;

// Callbacks
protected:
	UFUNCTION()
	void OnCharacterReady(ACharacter* InstigatorCharacter);
	
	UFUNCTION()
	void OnCharacterAbsorbShieldDamage(ACharacter* InstigatorCharacter, const float AbsorbedDamage, const float NewShieldValue);

	UFUNCTION()
	void OnCharacterTakeHealthDamage(ACharacter* InstigatorCharacter, const float TakenDamage, const float NewHealthValue);

	UFUNCTION()
	void OnCharacterRegenShield(ACharacter* InstigatorCharacter, const float RegenAmount, const float NewShieldValue);

	UFUNCTION()
	void OnCharacterRegenHealth(ACharacter* InstigatorCharacter, const float RegenAmount, const float NewHealthValue);

	UFUNCTION()
	void InitializeMappingContext();

protected:
	void SetupCharacter(AActor* NewCharacter);
	void InitializeHud();
	void InitializeHudDelegates();
	void FinalizeInitialize() const;
	
// Action functions
protected:
	void RequestJumpAction();
	void RequestStopJumpAction();
	void RequestMoveAction(const FInputActionValue&);
	void RequestLookAction(const FInputActionValue&);
	void RequestSprintAction();
	void RequestWeaponPullTriggerAction();
	void RequestWeaponReleaseTriggerAction();
	void RequestReloadWeapon();
	void RequestInteract();
	void RequestChangeWeaponFirstSlot();
	void RequestChangeWeaponSecondSlot();
	void RequestChangeWeaponThirdSlot();
	void RequestChangeWeaponFourthSlot();
	void RequestChangeWeapon(const int32 Index);

// Net functions
protected:
	UFUNCTION(Client, Reliable)
	void ClientOnCharacterHitSomeone();

	UFUNCTION(Client, Reliable)
	void ClientOnCharacterBrokeShield();

	UFUNCTION(Client, Reliable)
	void ClientOnCharacterKillSomeone(const int32 NewKillCount);

public:
	void OnCharacterHitSomeone();

	void OnCharacterKillSomeone(const int32 NewKillCount);

	void OnCharacterBrokeShield();

// Properties
protected:
	UPROPERTY(Transient)
	TObjectPtr<ABaseCharacter> BaseCharacterRef;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputMappingContext> MappingContext;

	UPROPERTY(EditAnywhere, Category="HUD")
	TSubclassOf<UPlayerHud> PlayerHudClass;

	UPROPERTY(Transient)
	TObjectPtr<UPlayerHud> PlayerHudRef;

	UPROPERTY(Transient)
	TObjectPtr<AKothArenaGameModeBase> GameModeRef;

	UPROPERTY(Transient)
	int32 KillCount = 0;
	
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
	
	UPROPERTY(EditDefaultsOnly, Category="Input")
    TObjectPtr<UInputAction> PullTriggerAction;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> ReleaseTriggerAction;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> ReloadWeaponAction;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> InteractAction;
	
	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> ChangeWeaponFirstSlotAction;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> ChangeWeaponSecondSlotAction;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> ChangeWeaponThirdSlotAction;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> ChangeWeaponFourthSlotAction;

};
