// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "ShooterCharacter.generated.h"

class UShieldComponent;
class UPlayerHud;

/**
 * 
 */
UCLASS()
class KOTHARENA_API AShooterCharacter : public ABaseCharacter
{
	GENERATED_BODY()

	
public:
	AShooterCharacter();

	virtual void BeginPlay() override;
	
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

protected:
	UPROPERTY(Transient)
	TObjectPtr<APlayerController> PC;
	
	UPROPERTY(EditAnywhere, Category="HUD")
	TSubclassOf<UPlayerHud> PlayerHudClass;

	UPROPERTY(Transient)
	TObjectPtr<UPlayerHud> PlayerHudRef;
};
