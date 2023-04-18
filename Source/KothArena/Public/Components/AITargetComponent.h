// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AITargetComponent.generated.h"

class UBlackboardComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KOTHARENA_API UAITargetComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAITargetComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void SelectTarget(const float);
	void PublishTargetInformation() const;
	
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
	TObjectPtr<UBlackboardComponent> BlackboardComponent;

	UPROPERTY(EditAnywhere, Category="Target")
	float TimeToUpdateTarget = 1.f;

	UPROPERTY()
	float CurrentTimeUpdateTarget = 0.f;
	
	UPROPERTY(Transient)
	TWeakObjectPtr<AActor> SelectedTarget;

	UPROPERTY(EditAnywhere, Category="Target")
	FName TargetObjectBlackboardKey = NAME_None;

	UPROPERTY(EditAnywhere, Category="Target")
	FName TargetLocationBlackboardKey = NAME_None;

	UPROPERTY(EditAnywhere, Category="Target")
	FName TargetDistanceBlackboardKey = NAME_None;
	
};
