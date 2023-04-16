// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BaseAIController.generated.h"

struct FAIStimulus;
class UAIPerceptionComponent;
class UBehaviorTree;
class UFindPlayerComponent;

/**
 * 
 */
UCLASS()
class KOTHARENA_API ABaseAIController : public AAIController
{
	GENERATED_BODY()

public:
	
protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

// Callbacks
protected:
	UFUNCTION()
	void OnPerceptionSomething(AActor* Actor, FAIStimulus Stimulus);
	
// Components
protected:
	UPROPERTY()
	TObjectPtr<UFindPlayerComponent> FindPlayerComponent;

	UPROPERTY()
	TObjectPtr<UAIPerceptionComponent> AiPerceptionComponent;

	
// AI Properties
protected:
	UPROPERTY(EditAnywhere, Category="Ai Properties")
	TObjectPtr<UBehaviorTree> BTAsset;

	UPROPERTY(EditAnywhere, Category="AI|Search Target")
	FName HasTargetKeyName = NAME_None;
	
	UPROPERTY(EditAnywhere, Category="AI|Search Target")
	FName TargetLocationKeyName = NAME_None;
};
