// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BaseAIController.generated.h"

struct FAIStimulus;
class UAIPerceptionComponent;
class UBehaviorTree;
class UFindPlayerComponent;
class UAITargetComponent;
class ABaseCharacter;

/**
 * 
 */
UCLASS()
class KOTHARENA_API ABaseAIController : public AAIController
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	void SetupCharacter(APawn* NewPawn);

	void ReleaseTargetTicket(AActor* Target);

// Callbacks
protected:
	UFUNCTION()
	void OnPerceptionSomething(AActor* Actor, FAIStimulus Stimulus);

	UFUNCTION()
	void OnCharacterReady(ACharacter* NewCharacter);

	UFUNCTION()
	void OnCharacterDeath(ACharacter* DeadCharacter, AController* KillerController);

	UFUNCTION()
	void OnChangeTarget(AActor* OldTarget, AActor* NewTarget);

// Public interface
public:
	UFUNCTION(BlueprintPure)
	AActor* GetCurrentTarget() const;
	
// Components
protected:
	UPROPERTY()
	TObjectPtr<UAITargetComponent> AITargetComponent;
	
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

	UPROPERTY(EditAnywhere, Category="AI|Target Ticket")
	FName HasTicketKeyName = NAME_None;

	UPROPERTY(Transient)
	TObjectPtr<ABaseCharacter> BaseCharacterRef;
};
