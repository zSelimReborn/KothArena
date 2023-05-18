// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Gameplay/Items/AmmoItem.h"
#include "BaseCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UHealthComponent;
class UShieldComponent;
class UWeaponInventoryComponent;
class UAmmoInventoryComponent;
class USearchItemComponent;
class UPlayerHud;
class ABaseWeapon;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterReady, ACharacter*, InstigatorCharacter);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAbsorbShieldDamageDelegate, ACharacter*, InstigatorCharacter, const float, DamageAbsorbed, const float, NewShieldValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnTakeHealthDamageDelegate, ACharacter*, InstigatorCharacter, const float, TakenDamage, const float, NewHealthValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnRegenShieldDelegate, ACharacter*, InstigatorCharacter, const float, RegenAmount, const float, NewShieldValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnRegenHealthDelegate, ACharacter*, InstigatorCharacter, const float, RegenAmount, const float, NewHealthValue);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTakeDamageDelegate, AController*, DamagedController, AController*, InstigatorController);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCharacterDeathDelegate, ACharacter*, DeadCharacter, AController*, KillerController);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCharacterShieldBrokenDelegate, ACharacter*, DamagedCharacter, AController*, ControllerCauser);

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
	virtual void PostInitializeComponents() override;

	void UpdateSprintStatus() const;
	
	float AbsorbShieldDamage(const float DamageAmount);

	float TakeHealthDamage(const float DamageAmount);
	
	void OnShieldBroken();

	void OnDeath();

	void RequestEquipDefaultWeapon();
	void RequestEquipWeapon(ABaseWeapon* NewWeapon);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void PossessedBy(AController* NewController) override;

	void InitializeCharacter();

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	void RequestMove(const FVector2d& AxisValue);
	void RequestLook(const FVector2d& AxisValue);
	void RequestToggleSprint() const;
	void HandleToggleSprint() const;
	void RequestJump();
	void RequestStopJumping();
	void RequestWeaponPullTrigger() const;
	void RequestWeaponReleaseTrigger() const;
	void RequestReloadCurrentWeapon();
	void RequestChangeWeapon(const int32 WeaponIndex) const;
	void RequestInteract();

	UFUNCTION(BlueprintPure)
	float GetMaxHealth() const;

	UFUNCTION(BlueprintPure)
	float GetCurrentHealth() const;

	UFUNCTION(BlueprintPure)
	bool HasShield() const;
	
	UFUNCTION(BlueprintPure)
	float GetMaxShield() const;

	UFUNCTION(BlueprintPure)
	float GetCurrentShield() const;

	UFUNCTION(BlueprintCallable)
	bool AddHealthRegen(const float HealthAmount);

	UFUNCTION(BlueprintCallable)
	bool AddShieldRegen(const float ShieldAmount);

	UFUNCTION(BlueprintCallable)
	bool AddAmmo(const EAmmoType AmmoType, const int32 Amount);

	UFUNCTION(BlueprintPure)
	bool HasAmmoInventory() const;
	
	UFUNCTION(BlueprintPure)
	int32 GetCurrentWeaponAmmo() const;

	UFUNCTION(BlueprintPure)
	int32 GetCurrentWeaponMaxAmmo() const;

	UFUNCTION(BlueprintPure)
	int32 GetCurrentWeaponAmmoStorage() const;

	UFUNCTION(BlueprintPure)
	EAmmoType GetCurrentWeaponAmmoType() const;

	UFUNCTION(BlueprintPure)
	ABaseWeapon* GetCurrentWeapon() const;

	void NotifyShieldDamage(const float DamageAbsorbed, const float NewShield);
	void NotifyShieldRegen(const float Amount, const float NewShield);
	void NotifyHealthDamage(const float DamageAbsorbed, const float NewHealth);
	void NotifyHealthRegen(const float Amount, const float NewHealth);

// Callbacks
protected:
	UFUNCTION()
	void OnNewItemFound(const FHitResult& HitResult, AActor* ItemFound);

	UFUNCTION()
	void OnItemLost(AActor* ItemLost);

// Net functions
protected:
	UFUNCTION(Server, Reliable)
	void ServerRequestEquipWeapon(ABaseWeapon* NewWeapon);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRequestEquipWeapon(ABaseWeapon* NewWeapon);

	UFUNCTION(Server, Reliable)
	void ServerRequestSprintToggle() const;
	
// Events
public:
	FOnCharacterReady& OnCharacterReady() { return CharacterReadyDelegate; }
	FOnAbsorbShieldDamageDelegate& OnAbsorbShieldDamage() { return AbsorbShieldDamageDelegate; }
	FOnTakeHealthDamageDelegate& OnTakeHealthDamage() { return TakeHealthDamageDelegate; }
	FOnRegenShieldDelegate& OnRegenShield() { return RegenShieldDelegate; }
	FOnRegenHealthDelegate& OnRegenHealth() { return RegenHealthDelegate; }
	FOnTakeDamageDelegate& OnTakeDamage() { return TakeDamageDelegate; }
	FOnCharacterDeathDelegate& OnCharacterDeath() { return CharacterDeathDelegate; }
	FOnCharacterShieldBrokenDelegate& OnCharacterShieldBroken() { return CharacterShieldBrokenDelegate; }
	
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

	UPROPERTY()
	TObjectPtr<UWeaponInventoryComponent> WeaponInventoryComponent;

	UPROPERTY()
	TObjectPtr<UAmmoInventoryComponent> AmmoInventoryComponent;

	UPROPERTY()
	TObjectPtr<USearchItemComponent> SearchItemComponent;
	
// Properties
protected:
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

	UPROPERTY(Transient)
	TObjectPtr<ABaseWeapon> WeaponFoundRef;

	UPROPERTY(Transient)
	TObjectPtr<AController> LastDamageCauserController;

// Events
protected:
	UPROPERTY()
	FOnCharacterReady CharacterReadyDelegate;
	
	UPROPERTY()
	FOnAbsorbShieldDamageDelegate AbsorbShieldDamageDelegate;

	UPROPERTY()
	FOnTakeHealthDamageDelegate TakeHealthDamageDelegate;

	UPROPERTY()
	FOnRegenShieldDelegate RegenShieldDelegate;

	UPROPERTY()
	FOnRegenHealthDelegate RegenHealthDelegate;

	UPROPERTY()
	FOnTakeDamageDelegate TakeDamageDelegate;

	UPROPERTY()
	FOnCharacterDeathDelegate CharacterDeathDelegate;

	UPROPERTY()
	FOnCharacterShieldBrokenDelegate CharacterShieldBrokenDelegate;
};
