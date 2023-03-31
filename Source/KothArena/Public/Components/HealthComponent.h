// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathDelegate);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KOTHARENA_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

// Component interface
public:
	UFUNCTION(BlueprintCallable)
	float TakeDamage(const float& Damage);

	UFUNCTION(BlueprintCallable)
	bool RegenHealth(const float& Amount);

	UFUNCTION(BlueprintPure)
	bool IsAlive() const;

	UFUNCTION(BlueprintPure)
	float GetCurrentHealth() const;

	UFUNCTION(BlueprintPure)
	float GetMaxHealth() const;

	UFUNCTION(BlueprintPure)
	float GetCurrentHealthPercentage() const;

	FOnDeathDelegate& OnDeathEvent() { return OnDeathDelegate; }
	
// Properties
protected:
	UPROPERTY(EditAnywhere, Category="Health")
	float MaxHealth = 100.f;

	UPROPERTY(VisibleAnywhere, Category="Health")
	float CurrentHealth = 0.f;

	FOnDeathDelegate OnDeathDelegate;
};
