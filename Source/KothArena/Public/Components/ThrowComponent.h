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

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

// Internal functions
protected:
	APlayerController* GetPlayerController();
	
	void UpdateCharge(const float DeltaTime);

	ABaseThrowable* SpawnAndAttachThrowable() const;
	
// Component interface
public:
	UFUNCTION(BlueprintCallable)
	ABaseThrowable* StartThrowing();

	UFUNCTION(BlueprintCallable)
	void FinishThrowing();

	UFUNCTION(BlueprintCallable)
	void ChangeThrowable(const TSubclassOf<ABaseThrowable> NewThrowableClass);

// Properties
protected:
	UPROPERTY(EditAnywhere, Category="Throwable")
	TSubclassOf<ABaseThrowable> ThrowableClass;

	UPROPERTY(EditAnywhere, Category="Throwable")
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

	UPROPERTY(VisibleAnywhere, Category="Throwable")
	float CurrentMagnitude = MinThrowMagnitude;

	UPROPERTY(VisibleAnywhere, Category="Throwable")
	EThrowActionState ActionState = EThrowActionState::Idle;
	
	UPROPERTY()
	TObjectPtr<ABaseCharacter> BaseCharacterRef;

	UPROPERTY()
	TObjectPtr<APlayerController> PC;
};
