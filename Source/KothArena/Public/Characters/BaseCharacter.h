// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Gameplay/Items/AmmoItem.h"
#include "Gameplay/Weapons/BaseWeapon.h"
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
class AThrowableItem;
class ABaseThrowable;
class UAimComponent;
class UChildActorComponent;
class ACameraActor;
class UTicketComponent;

UENUM(BlueprintType)
enum class ECharacterCombatState : uint8
{
	Idle,
	Aiming,
	Reloading,
	Throwing
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterReady, ACharacter*, InstigatorCharacter);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAbsorbShieldDamageDelegate, ACharacter*, InstigatorCharacter, const float, DamageAbsorbed, const float, NewShieldValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnTakeHealthDamageDelegate, ACharacter*, InstigatorCharacter, const float, TakenDamage, const float, NewHealthValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnRegenShieldDelegate, ACharacter*, InstigatorCharacter, const float, RegenAmount, const float, NewShieldValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnRegenHealthDelegate, ACharacter*, InstigatorCharacter, const float, RegenAmount, const float, NewHealthValue);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTakeDamageDelegate, AController*, DamagedController, AController*, InstigatorController);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCharacterDeathDelegate, ACharacter*, DeadCharacter, AController*, KillerController);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCharacterShieldBrokenDelegate, ACharacter*, DamagedCharacter, AController*, ControllerCauser);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnCharacterChangeThrowableDelegate,
	TSubclassOf<ABaseThrowable>, NewThrowableClass, const int32, Quantity
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnCharacterChangeThrowableQuantityDelegate,
	const int32, NewQuantity
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnCharacterEquipWeaponDelegate,
	ABaseWeapon*,
	EquippedWeapon
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnCharacterChangeWeaponDelegate,
	ABaseWeapon*,
	EquippedWeapon
);

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
	
	void HandleRequestChangeThrowable(const AThrowableItem* NewThrowableClass, const int32 Quantity);

	void HandleRequestAddThrowableQuantity(const int32 Quantity);
	
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
	void RequestWeaponPullTrigger();
	void RequestWeaponReleaseTrigger();
	void RequestReloadCurrentWeapon();
	void RequestChangeWeapon(const int32 WeaponIndex) const;
	void RequestInteract();
	void RequestStartAiming();
	void RequestEndAiming();
	void RequestStartThrowing();
	void RequestFinishThrowing();
	void RequestAddThrowableQuantity(const int32 Quantity);

	void PrepareForBattle();

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
	bool ShouldReload() const;
	
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
	EWeaponType GetCurrentWeaponType() const;

	UFUNCTION(BlueprintPure)
	float GetCurrentWeaponRecoilAngle() const;

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

	UFUNCTION(BlueprintPure)
	TSubclassOf<ABaseThrowable> GetCurrentThrowable() const;

	void NotifyShieldDamage(const float DamageAbsorbed, const float NewShield);
	void NotifyShieldRegen(const float Amount, const float NewShield);
	void NotifyHealthDamage(const float DamageAbsorbed, const float NewHealth);
	void NotifyHealthRegen(const float Amount, const float NewHealth);

	void RequestEquipDefaultWeapon();

	void SetAimingState();
	void UnsetAimingState();
	
	UFUNCTION(BlueprintPure)
	ACameraActor* GetAimCamera() const;

	UFUNCTION(BlueprintPure)
	AActor* GetLastDamageCauser() const;

	UFUNCTION(BlueprintPure)
	FORCEINLINE int32 GetTeamId() const { return TeamId; }

	void SetTeamId(const int32);

	UFUNCTION(BlueprintCallable)
	TArray<AActor*> GetTeamMembers();
	
	void FillTeamMembers();

	UFUNCTION(BlueprintCallable)
	bool ClaimTicket(const AController* Claimer);

	UFUNCTION(BlueprintCallable)
	void ReleaseTicket(const AController* Claimer);

// Callbacks
protected:
	UFUNCTION()
	void OnNewItemFound(const FHitResult& HitResult, AActor* ItemFound);

	UFUNCTION()
	void OnItemLost(AActor* ItemLost);

	UFUNCTION()
	void OnChangeThrowable(TSubclassOf<ABaseThrowable> NewThrowableClass, const int32 Quantity);

	UFUNCTION()
	void OnChangeThrowableQuantity(const int32 NewQuantity);

	UFUNCTION()
	void OnEquipWeapon(ABaseWeapon* EquippedWeapon);

	UFUNCTION()
	void OnChangeWeapon(ABaseWeapon* NewWeapon);

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

	UFUNCTION(Server, Reliable)
	void ServerRequestChangeThrowable(AThrowableItem* NewThrowableClass, const int32 Quantity);

	UFUNCTION(Server, Reliable)
	void ServerRequestAddThrowableQuantity(const int32 Quantity);

	UFUNCTION(Server, Reliable)
	void ServerRequestStartThrowing();

	UFUNCTION(Server, Reliable)
	void ServerRequestFinishThrowing();
	
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
	FOnCharacterChangeThrowableDelegate& OnCharacterChangeThrowable() { return CharacterChangeThrowableDelegate; }
	FOnCharacterChangeThrowableQuantityDelegate& OnCharacterChangeThrowableQuantity() { return CharacterChangeThrowableQuantityDelegate; }
	FOnCharacterEquipWeaponDelegate& OnCharacterEquipWeapon() { return CharacterEquipWeaponDelegate; }
	FOnCharacterChangeWeaponDelegate& OnCharacterChangeWeapon() { return CharacterChangeWeaponDelegate; }
	
// Components
protected:
	UPROPERTY(VisibleAnywhere, NoClear)
	TObjectPtr<USpringArmComponent> CameraBoom;
	
	UPROPERTY(VisibleAnywhere, NoClear)
	TObjectPtr<UCameraComponent> CameraComponent;

	UPROPERTY(VisibleAnywhere, NoClear)
	TObjectPtr<USpringArmComponent> AimCameraBoom;
	
	UPROPERTY(VisibleAnywhere, NoClear)
	TObjectPtr<UChildActorComponent> ChildAimCameraComponent;

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

	UPROPERTY()
	TObjectPtr<UAimComponent> AimComponent;

	UPROPERTY()
	TObjectPtr<UTicketComponent> TicketComponent;
	
// Properties
protected:
	UPROPERTY(EditAnywhere, Category="Look", meta=(ClampMin="0", UIMin="0", ForceUnits="deg/s"))
	float BaseLookUpRate = 90.f;

	UPROPERTY(EditAnywhere, Category="Look", meta=(ClampMin="0", UIMin="0", ForceUnits="deg/s"))
	float BaseLookRightRate = 90.f;

	UPROPERTY(EditAnywhere, Category="Sprint", meta=(ClampMin="0", UIMin="0", ForceUnits="cm/s"))
	float SprintSpeed = 900.f;
	
	UPROPERTY(EditAnywhere, Category="Aiming")
	float AimingLookUpRate = 50.f;

	UPROPERTY(EditAnywhere, Category="Aiming")
	float AimingLookRightRate = 50.f;

	UPROPERTY(EditAnywhere, Category="Aiming")
	float AimAssistLookUpRate = 25.f;

	UPROPERTY(EditAnywhere, Category="Aiming")
	float AimAssistLookRightRate = 25.f;

	UPROPERTY(EditAnywhere, Category="Aiming")
	float AimingWalkSpeed = 250.f;

	UPROPERTY(EditAnywhere, Category="Throwable")
	FName ThrowableSocketName = NAME_None;

	UPROPERTY(EditAnywhere, Category="Team", Replicated)
	int32 TeamId;

	UPROPERTY(Transient)
	float IdleFov = 90.f;
	
	UPROPERTY(Transient)
	float WalkSpeed = 600.f;

	UPROPERTY(Transient)
	TObjectPtr<APlayerController> PC;

	UPROPERTY(Transient)
	TObjectPtr<AActor> ItemFoundRef;
	
	UPROPERTY(Transient)
	TObjectPtr<ABaseWeapon> WeaponFoundRef;

	UPROPERTY(Transient)
	TObjectPtr<AThrowableItem> ThrowableItemFoundRef;

	UPROPERTY(Transient)
	TObjectPtr<AController> LastDamageCauserController;

	UPROPERTY(Transient, Replicated)
	bool bDefaultWeaponSpawned = false;

	UPROPERTY(Transient, Replicated)
	ECharacterCombatState CombatState = ECharacterCombatState::Idle;

	UPROPERTY()
	TArray<AActor*> TeamMembers;

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

	UPROPERTY()
	FOnCharacterChangeThrowableDelegate CharacterChangeThrowableDelegate;

	UPROPERTY()
	FOnCharacterChangeThrowableQuantityDelegate CharacterChangeThrowableQuantityDelegate;

	UPROPERTY()
	FOnCharacterEquipWeaponDelegate CharacterEquipWeaponDelegate;

	UPROPERTY()
	FOnCharacterChangeWeaponDelegate CharacterChangeWeaponDelegate;
};
