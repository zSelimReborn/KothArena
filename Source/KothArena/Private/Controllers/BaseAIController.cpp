// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/BaseAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/BaseCharacter.h"
#include "Components/AITargetComponent.h"
#include "Perception/AIPerceptionComponent.h"

void ABaseAIController::BeginPlay()
{
	Super::BeginPlay();
	AiPerceptionComponent = FindComponentByClass<UAIPerceptionComponent>();
	AITargetComponent = FindComponentByClass<UAITargetComponent>();
	if (AiPerceptionComponent)
	{
		AiPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ABaseAIController::OnPerceptionSomething);
	}
}

void ABaseAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	RunBehaviorTree(BTAsset);
	SetupCharacter(InPawn);
}

void ABaseAIController::SetupCharacter(APawn* NewPawn)
{
	if (NewPawn == nullptr)
	{
		return;
	}

	BaseCharacterRef = Cast<ABaseCharacter>(NewPawn);
	if (BaseCharacterRef == nullptr)
	{
		return;
	}

	OnCharacterReady(BaseCharacterRef);
	if (!BaseCharacterRef->OnCharacterReady().IsAlreadyBound(this, &ABaseAIController::OnCharacterReady))
	{
		BaseCharacterRef->OnCharacterReady().AddDynamic(this, &ABaseAIController::OnCharacterReady);
	}
}

void ABaseAIController::OnPerceptionSomething(AActor* Actor, FAIStimulus Stimulus)
{
	if (ABaseCharacter* CharacterDetected = Cast<ABaseCharacter>(Actor))
	{
		if (CharacterDetected)
		{
			GetBlackboardComponent()->SetValueAsBool(HasTargetKeyName, Stimulus.WasSuccessfullySensed());
			GetBlackboardComponent()->SetValueAsVector(TargetLocationKeyName, CharacterDetected->GetActorLocation());
		}
	}
}

void ABaseAIController::OnCharacterReady(ACharacter* NewCharacter)
{
	if (BaseCharacterRef == nullptr)
	{
		return;
	}

	BaseCharacterRef->PrepareForBattle();
}

AActor* ABaseAIController::GetCurrentTarget() const
{
	if (AITargetComponent == nullptr)
	{
		return nullptr;
	}

	return AITargetComponent->GetCurrentTarget();
}
