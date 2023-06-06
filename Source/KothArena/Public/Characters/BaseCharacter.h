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
class UThrowComponent;
class UPlayerHud;
class ABaseWeapon;

UENUM(BlueprintType)
enum class ECharacterCombatState : uint8
{
	Idle,
	Aiming,
	Reloading
};

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
	
	void RequestEquipWeapon(ABaseWeapon* NewWeapon);

	void HandleToggleSprint();

	void HandleRequestStartAiming();

	void HandleRequestEndAiming();

	void HandleRequestReload();

	void UpdateAim(const float);
	
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
	void RequestToggleSprint();
	void RequestJump();
	void RequestStopJumping();
	void RequestWeaponPullTrigger() const;
	void RequestWeaponReleaseTrigger() const;
	void RequestReloadCurrentWeapon();
	void RequestChangeWeapon(const int32 WeaponIndex) const;
	void RequestInteract();
	void RequestStartAiming();
	void RequestEndAiming();
	void RequestStartThrowing();
	void RequestFinishThrowing();

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

	UFUNCTION(BlueprintPure)
	FORCEINLINE ECharacterCombatState GetCharacterCombatState() const { return CombatState; };

	UFUNCTION(BlueprintPure)
	FORCEINLINE bool IsAiming() const { return GetCharacterCombatState() == ECharacterCombatState::Aiming; }

	UFUNCTION(BlueprintPure)
	float GetLookUpRate() const;

	UFUNCTION(BlueprintPure)
	float GetLookRightRate() const;

	UFUNCTION(BlueprintPure)
	FName GetThrowableSocketName() const { return ThrowableSocketName; }

	void NotifyShieldDamage(const float DamageAbsorbed, const float NewShield);
	void NotifyShieldRegen(const float Amount, const float NewShield);
	void NotifyHealthDamage(const float DamageAbsorbed, const float NewHealth);
	void NotifyHealthRegen(const float Amount, const float NewHealth);

	void RequestEquipDefaultWeapon();

// Callbacks
protected:
	UFUNCTION()
	void OnNewItemFound(const FHitResult& HitResult, AActor* ItemFound);

	UFUNCTION()
	void OnItemLost(AActor* ItemLost);

// Net functions
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
protected:
	UFUNCTION(Server, Reliable)
	void ServerRequestEquipWeapon(ABaseWeapon* NewWeapon);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRequestEquipWeapon(ABaseWeapon* NewWeapon);

	UFUNCTION(Server, Reliable)
	void ServerRequestSprintToggle();

	UFUNCTION(Server, Reliable)
	void ServerRequestStartAiming();

	UFUNCTION(Server, Reliable)
	void ServerRequestEndAiming();

	UFUNCTION(Server, Reliable)
	void ServerRequestReload();
	
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

	UPROPERTY()
	TObjectPtr<UThrowComponent> ThrowComponent;
	
// Properties
protected:
	UPROPERTY(EditAnywhere, Category="Look", meta=(ClampMin="0", UIMin="0", ForceUnits="deg/s"))
	float BaseLookUpRate = 90.f;

	UPROPERTY(EditAnywhere, Category="Look", meta=(ClampMin="0", UIMin="0", ForceUnits="deg/s"))
	float BaseLookRightRate = 90.f;

	UPROPERTY(EditAnywhere, Category="Sprint", meta=(ClampMin="0", UIMin="0", ForceUnits="cm/s"))
	float SprintSpeed = 900.f;

	UPROPERTY(EditAnywhere, Category="Aiming")
	float AimingFov = 50.f;

	UPROPERTY(EditAnywhere, Category="Aiming")
	float TimeToAim = 1.f;

	UPROPERTY(VisibleAnywhere, Category="Aiming")
	float CurrentTimeAiming = 0.f;

	UPROPERTY(EditAnywhere, Category="Aiming")
	float AimingLookUpRate = 50.f;

	UPROPERTY(EditAnywhere, Category="Aiming")
	float AimingLookRightRate = 50.f;

	UPROPERTY(EditAnywhere, Category="Aiming")
	float AimingWalkSpeed = 250.f;

	UPROPERTY(EditAnywhere, Category="Throwable")
	FName ThrowableSocketName = NAME_None;

	UPROPERTY(Transient)
	float IdleFov = 90.f;
	
	UPROPERTY(Transient)
	float WalkSpeed = 600.f;

	UPROPERTY(Transient)
	TObjectPtr<APlayerController> PC;

	UPROPERTY(Transient)
	TObjectPtr<ABaseWeapon> WeaponFoundRef;

	UPROPERTY(Transient)
	TObjectPtr<AController> LastDamageCauserController;

	UPROPERTY(Transient, Replicated)
	bool bDefaultWeaponSpawned = false;

	UPROPERTY(Transient, Replicated)
	ECharacterCombatState CombatState = ECharacterCombatState::Idle;

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
