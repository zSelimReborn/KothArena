// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "KothArenaGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class KOTHARENA_API AKothArenaGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

// Delegates
protected:
	UFUNCTION()
	void OnCharacterTakeDamage(AController* ControllerDamaged, AController* ControllerCauser);

	UFUNCTION()
	void OnCharacterDeath(ACharacter* Character, AController* ControllerCauser);
	
};
