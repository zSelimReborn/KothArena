// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/Items/ConsumableItem.h"
#include "AmmoItem.generated.h"

UENUM(BlueprintType)
enum class EAmmoType : uint8
{
	Default			UMETA(DisplayName="None"),
	LargeCaliber	UMETA(DisplayName="Large Caliber"),
	SmallCaliber	UMETA(DisplayName="Small Caliber"),
	Shotgun			UMETA(DisplayName="Shot shell"),
	Missile			UMETA(DisplayName="Missile")
};

UCLASS()
class KOTHARENA_API AAmmoItem : public AConsumableItem
{
	GENERATED_BODY()

protected:
	int32 GetAmountToAdd() const;
	
public:
	virtual bool ConsumeItem(AActor* InstigatorActor) override;

// Properties
protected:
	UPROPERTY(EditAnywhere, Category="Ammo")
	EAmmoType AmmoType = EAmmoType::LargeCaliber;

	UPROPERTY(EditAnywhere, Category="Ammo")
	int32 MinAmount = 30;

	UPROPERTY(EditAnywhere, Category="Ammo")
	int32 MaxAmount = 55;
};
