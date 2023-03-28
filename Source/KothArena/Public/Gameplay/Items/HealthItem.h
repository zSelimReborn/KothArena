// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ConsumableItem.h"
#include "HealthItem.generated.h"

/**
 * 
 */
UCLASS()
class KOTHARENA_API AHealthItem : public AConsumableItem
{
	GENERATED_BODY()

protected:
	int32 GetHealthAmountToRegen() const;
	
public:
	virtual bool ConsumeItem(AActor* InstigatorActor) override;
	
// Properties
protected:
	UPROPERTY(EditAnywhere, Category="Item|Health")
	int32 MinHealthRegen = 5;

	UPROPERTY(EditAnywhere, Category="Item|Health")
	int32 MaxHealthRegen = 15;
};
