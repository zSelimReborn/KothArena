// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/Items/ConsumableItem.h"
#include "ThrowableItem.generated.h"

class UWidgetComponent;
class ABaseThrowable;

/**
 * 
 */
UCLASS()
class KOTHARENA_API AThrowableItem : public AConsumableItem
{
	GENERATED_BODY()

public:
	AThrowableItem();
	virtual void BeginPlay() override;

	virtual bool ConsumeItem(AActor* InstigatorActor) override;
	virtual void EnableHighlight() const override;
	virtual void DisableHighlight() const override;

	TSubclassOf<ABaseThrowable> GetThrowableClass() const { return ThrowableClass; }
	int32 GetQuantity() const { return Quantity; }

// Components
protected:
	UPROPERTY(VisibleAnywhere, NoClear)
	TObjectPtr<UWidgetComponent> PickupThrowableWidgetComponent;

// Properties
protected:
	UPROPERTY(EditAnywhere, Category="Throwable")
	TSubclassOf<ABaseThrowable> ThrowableClass;

	UPROPERTY(EditAnywhere, Category="Throwable")
	int32 Quantity = 1;
};
