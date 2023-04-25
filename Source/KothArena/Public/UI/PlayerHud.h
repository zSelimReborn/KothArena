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
	void InitializeOwnerController(AController* OwnerController);
	
	UFUNCTION(BlueprintImplementableEvent)
	void InitializeHealthAndShield(bool bHasHealth, const float MaxHealth, const float CurrentHealth, bool bHasShield, const float MaxShield, const float CurrentShield);

	UFUNCTION(BlueprintImplementableEvent)
	void OnAbsorbShieldDamage(const float Damage, const float NewShieldValue);
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnTakeHealthDamage(const float Damage, const float NewHealthValue);

	UFUNCTION(BlueprintImplementableEvent)
	void OnRegenShield(const float RegenAmount, const float NewShieldValue);

	UFUNCTION(BlueprintImplementableEvent)
	void OnRegenHealth(const float RegenAmount, const float NewHealthValue);
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnPlayerDeath();

	UFUNCTION(BlueprintImplementableEvent)
	void OnHitSomeone();

	UFUNCTION(BlueprintImplementableEvent)
	void OnKillSomeone(const int32 KillCounter);

	UFUNCTION(BlueprintImplementableEvent)
	void OnBrokeShield();
};
