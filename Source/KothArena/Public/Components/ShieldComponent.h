// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ShieldComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnShieldBreakDelegate);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KOTHARENA_API UShieldComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UShieldComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

// Component interface
public:
	UFUNCTION(BlueprintCallable)
	float AbsorbDamage(const float& Damage);

	UFUNCTION(BlueprintCallable)
	bool RegenShield(const float& Amount);

	UFUNCTION(BlueprintPure)
	bool IsBroken() const;

	UFUNCTION(BlueprintPure)
	float GetMaxShield() const;

	UFUNCTION(BlueprintPure)
	float GetCurrentShield() const;

	UFUNCTION(BlueprintPure)
	float GetCurrentShieldPercentage() const;

	FOnShieldBreakDelegate& OnShieldBreakEvent() { return OnShieldBreakDelegate; };

// Properties
protected:
	UPROPERTY(EditAnywhere, Category="Shield")
	float MaxShield = 100.f;

	UPROPERTY(VisibleAnywhere, Category="Shield")
	float CurrentShield = 0.f;

	FOnShieldBreakDelegate OnShieldBreakDelegate;
};
