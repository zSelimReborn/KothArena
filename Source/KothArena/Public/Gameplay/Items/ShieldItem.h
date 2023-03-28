// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ConsumableItem.h"
#include "ShieldItem.generated.h"

/**
 * 
 */
UCLASS()
class KOTHARENA_API AShieldItem : public AConsumableItem
{
	GENERATED_BODY()

protected:
	int32 GetShieldAmountToRegen() const;
	
public:
	virtual bool ConsumeItem(AActor* InstigatorActor) override;
	
// Properties
protected:
	UPROPERTY(EditAnywhere, Category="Item|Shield")
	int32 MinShieldRegen = 5;

	UPROPERTY(EditAnywhere, Category="Item|Shield")
	int32 MaxShieldRegen = 15;
};
