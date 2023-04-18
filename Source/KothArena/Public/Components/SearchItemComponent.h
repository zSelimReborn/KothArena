// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SearchItemComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnNewItemFoundDelegate, const FHitResult&, HitResult, AActor*, NewItem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemLostDelegate, AActor*, LostItem);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KOTHARENA_API USearchItemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USearchItemComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	float GetAngleBetweenVectors(FVector, FVector) const;
	void SearchForItems();
	
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FOnNewItemFoundDelegate& OnNewItemFound() { return NewItemFoundDelegate; }
	FOnItemLostDelegate& OnItemLost() { return ItemLostDelegate; }
	
// Properties
protected:
	UPROPERTY(EditAnywhere, Category="Trace")
	bool bShouldSearchForItems = true;
	
	UPROPERTY(EditAnywhere, Category="Trace")
	float SearchTraceLength = 1000.f;

	UPROPERTY(EditAnywhere, Category="Trace")
	float SearchMaxFOV = 90.f;

	UPROPERTY(Transient, VisibleAnywhere, Category="Trace")
	TObjectPtr<APawn> OwnerPawn;

	UPROPERTY(Transient, VisibleAnywhere, Category="Trace")
	TObjectPtr<AActor> ItemFoundRef;

// Delegates
protected:
	FOnNewItemFoundDelegate NewItemFoundDelegate;

	FOnItemLostDelegate ItemLostDelegate;
};
