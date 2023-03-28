// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/Items/BaseItem.h"
#include "ConsumableItem.generated.h"

class USphereComponent;
class URotatingMovementComponent;

/**
 * 
 */
UCLASS()
class KOTHARENA_API AConsumableItem : public ABaseItem
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	AConsumableItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

// Callbacks
protected:
	UFUNCTION()
	virtual void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	virtual bool ConsumeItem(AActor* InstigatorActor);
	
// Components
protected:
	UPROPERTY(VisibleAnywhere, NoClear)
	TObjectPtr<UStaticMeshComponent> BaseMeshComponent;

	UPROPERTY(VisibleAnywhere, NoClear)
	TObjectPtr<USphereComponent> TriggerVolume;

	UPROPERTY(VisibleAnywhere, NoClear)
	TObjectPtr<URotatingMovementComponent> RotatingMovementComponent;

// Properties
protected:
	UPROPERTY(EditAnywhere, Category="Item")
	bool bDestroyOnConsume = true;

	UPROPERTY(EditAnywhere, Category="Item")
	bool bConsumeOnOverlap = true;
};
