// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseItem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FItemUsedDelegate, AActor*, ItemUsed, AActor*, Instigator);

UCLASS()
class KOTHARENA_API ABaseItem : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	ABaseItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FItemUsedDelegate& OnItemUsed() { return ItemUsedDelegate; }

	FORCEINLINE bool IsItemEnabled() const { return bIsItemEnabled; }
	FORCEINLINE void DisableItem() { bIsItemEnabled = false; }
	FORCEINLINE void EnableItem() { bIsItemEnabled = true; }
	
// Components
protected:
	UPROPERTY(VisibleAnywhere, NoClear)
	TObjectPtr<USceneComponent> DefaultSceneComponent;
	
// Properties
protected:
	FItemUsedDelegate ItemUsedDelegate;

	UPROPERTY(VisibleAnywhere, Category="Item")
	bool bIsItemEnabled = true;
};
