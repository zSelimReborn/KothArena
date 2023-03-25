// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "ShooterCharacter.generated.h"

class UShieldComponent;

/**
 * 
 */
UCLASS()
class KOTHARENA_API AShooterCharacter : public ABaseCharacter
{
	GENERATED_BODY()

	
public:
	AShooterCharacter();
	
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintPure)
	float GetMaxShield() const;

	UFUNCTION(BlueprintPure)
	float GetCurrentShield() const;

protected:
	void OnShieldBroken();

// Components
protected:
	UPROPERTY(VisibleAnywhere, NoClear)
	TObjectPtr<UShieldComponent> ShieldComponent;
};
