// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHud.generated.h"

/**
 * 
 */
UCLASS()
class KOTHARENA_API UPlayerHud : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void SetCurrentShield(const float& ShieldValue);

	UFUNCTION(BlueprintImplementableEvent)
	void SetMaxShield(const float& MaxShieldValue);

	UFUNCTION(BlueprintImplementableEvent)
	void SetCurrentHealth(const float& HealthValue);

	UFUNCTION(BlueprintImplementableEvent)
	void SetMaxHealth(const float& MaxHealth);
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnTakeDamage(const float& Damage, const float& HealthValue, const float& ShieldValue);

	UFUNCTION(BlueprintImplementableEvent)
	void OnRegen(const float& RegenAmount, const float& HealthValue, const float& ShieldValue);
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnPlayerDeath();
};
