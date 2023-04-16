// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FindPlayerComponent.generated.h"

class UBlackboardComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KOTHARENA_API UFindPlayerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFindPlayerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void UpdatePlayerLocation() const;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

// Properties
protected:
	UPROPERTY(Transient)
	TObjectPtr<AController> OwnerController;
	
	UPROPERTY(Transient)
	TObjectPtr<APawn> PossessedPawn;
	
	UPROPERTY(Transient)
	TObjectPtr<APawn> PlayerRef;

	UPROPERTY(Transient)
	TObjectPtr<UBlackboardComponent> BlackboardComponent;

	UPROPERTY(EditAnywhere)
	FName PlayerObjectBlackboardKey = NAME_None;

	UPROPERTY(EditAnywhere)
	FName PlayerLocationBlackboardKey = NAME_None;

	UPROPERTY(EditAnywhere)
	FName RangeToPlayerBlackboardKey = NAME_None;
};
