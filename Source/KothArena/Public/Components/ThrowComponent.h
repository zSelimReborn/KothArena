// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ThrowComponent.generated.h"

class ABaseThrowable;
class ABaseCharacter;

UENUM(BlueprintType)
enum class EThrowActionState : uint8
{
	Idle,
	Charge
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnChangeThrowableDelegate,
	TSubclassOf<ABaseThrowable>, NewThrowableClass, const int32, Quantity
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnChangeQuantityDelegate,
	const int32, NewQuantity
);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KOTHARENA_API UThrowComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UThrowComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	FVector ComputeLaunchVelocity(const APlayerController*, const FVector& StartLocation) const;
	FVector GetHandLocation() const;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

// Internal functions
protected:
	APlayerController* GetPlayerController();
	
	void UpdateCharge(const float DeltaTime);
	void UpdateTrajectory();

	ABaseThrowable* SpawnAndAttachThrowable() const;
	
// Component interface
public:
	UFUNCTION(BlueprintCallable)
	ABaseThrowable* StartThrowing();

	UFUNCTION(BlueprintCallable)
	void FinishThrowing();

	UFUNCTION(BlueprintCallable)
	void ChangeThrowable(const TSubclassOf<ABaseThrowable> NewThrowableClass, const int32 Quantity);

	UFUNCTION(BlueprintCallable)
	void AddQuantity(const int32 Quantity);

	UFUNCTION(BlueprintPure)
	TSubclassOf<ABaseThrowable> GetCurrentThrowable() const { return ThrowableClass; }

	UFUNCTION(BlueprintPure)
	int32 GetQuantity() const { return ThrowableInventory; }

// Events
public:
	FOnChangeThrowableDelegate& OnChangeThrowable() { return OnChangeThrowableDelegate; }
	FOnChangeQuantityDelegate& OnChangeQuantity() { return OnChangeQuantityDelegate; }
	
// Net functions
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_ThrowableClass();

	UFUNCTION()
	void OnRep_ThrowableInventory();

// Properties
protected:
	UPROPERTY(EditAnywhere, Category="Throwable", ReplicatedUsing=OnRep_ThrowableClass)
	TSubclassOf<ABaseThrowable> ThrowableClass;

	UPROPERTY(EditAnywhere, Category="Throwable", ReplicatedUsing=OnRep_ThrowableInventory)
	int32 ThrowableInventory = 0;

	UPROPERTY()
	TObjectPtr<ABaseThrowable> CurrentThrowable;

	UPROPERTY(EditAnywhere, Category="Throwable")
	float MinThrowMagnitude = 800.f;

	UPROPERTY(EditAnywhere, Category="Throwable")
	float MaxThrowMagnitude = 1500.f;

	UPROPERTY(EditAnywhere, Category="Throwable")
	float SpeedToReachMaxMagnitude = 1.f;

	UPROPERTY(VisibleAnywhere, Category="Throwable")
	float CurrentSpeedAccumulator = 0.f;

	UPROPERTY(VisibleAnywhere, Category="Throwable", Replicated)
	float CurrentMagnitude = MinThrowMagnitude;

	UPROPERTY(EditAnywhere, Category="Throwable")
	float ZValueModifierPercentage = 1.3f;

	UPROPERTY(EditAnywhere, Category="Throwable")
	bool bShowTrajectory = true;

	UPROPERTY(VisibleAnywhere, Category="Throwable", Replicated)
	EThrowActionState ActionState = EThrowActionState::Idle;
	
	UPROPERTY()
	TObjectPtr<ABaseCharacter> BaseCharacterRef;

	UPROPERTY()
	TObjectPtr<APlayerController> PC;

// Delegates
protected:
	FOnChangeThrowableDelegate OnChangeThrowableDelegate;
	FOnChangeQuantityDelegate OnChangeQuantityDelegate;
};
