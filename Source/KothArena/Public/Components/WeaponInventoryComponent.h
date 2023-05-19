// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponInventoryComponent.generated.h"

class ABaseCharacter;
class ABaseWeapon;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KOTHARENA_API UWeaponInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWeaponInventoryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	void UnEquipCurrentWeapon();

	int32 AddWeapon(ABaseWeapon* Weapon);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

// Weapon Inventory Interface
public:
	ABaseWeapon* EquipDefaultWeapon();

	UFUNCTION(BlueprintCallable)
	bool EquipWeapon(ABaseWeapon* Weapon);

	UFUNCTION(BlueprintCallable)
	bool ChangeWeapon(const int32 WeaponIndex);

	UFUNCTION(BlueprintCallable)
	void PullTrigger();

	UFUNCTION(BlueprintCallable)
	void ReleaseTrigger();

	FORCEINLINE ABaseWeapon* GetCurrentWeapon() { return CurrentWeaponRef; };

	UFUNCTION(BlueprintPure)
	FORCEINLINE bool ShouldSpawnDefaultWeaponOnBeginPlay() const { return bEquipDefaultWeaponOnBegin; }

// Net functions
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION()
	void OnRep_DefaultWeaponRef();
	
// Properties
protected:
	UPROPERTY(Transient)
	TObjectPtr<ABaseCharacter> BaseCharacterRef;

	UPROPERTY(Transient)
	TObjectPtr<ABaseWeapon> CurrentWeaponRef;

	UPROPERTY(EditAnywhere, Category="Weapon")
	FName ShooterSocketAttachName = NAME_None;
	
	UPROPERTY(EditAnywhere, Category="Weapon")
	bool bEquipDefaultWeaponOnBegin = true;

	UPROPERTY(EditAnywhere, Category="Weapon")
	TSubclassOf<ABaseWeapon> DefaultWeaponClass;

	UPROPERTY(EditAnywhere, Category="Weapon")
	int32 InventoryCapacity = 4;

	UPROPERTY(VisibleAnywhere, Category="Weapon")
	int32 CurrentWeaponIndex = 0;
	
	UPROPERTY(VisibleAnywhere, Category="Weapon")
	TArray<TObjectPtr<ABaseWeapon>> WeaponInventory;

	UPROPERTY(Transient, ReplicatedUsing=OnRep_DefaultWeaponRef)
	TObjectPtr<ABaseWeapon> DefaultWeaponRef;
};
