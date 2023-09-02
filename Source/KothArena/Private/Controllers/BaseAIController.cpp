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
	if (AiPerceptionComponent != nullptr)
	{
		AiPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ABaseAIController::OnPerceptionSomething);
	}

	if (AITargetComponent != nullptr)
	{
		AITargetComponent->OnChangeTarget().AddDynamic(this, &ABaseAIController::OnChangeTarget);
	}
}

void ABaseAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	if (bStartAILogicOnPossess)
	{
		StartBehaviour();
	}
	
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

	BaseCharacterRef->OnCharacterDeath().AddDynamic(this, &ABaseAIController::OnCharacterDeath);
}

void ABaseAIController::ReleaseTargetTicket(AActor* Target)
{
	if (Target == nullptr)
	{
		return;
	}

	//UE_LOG(LogTemp, Error, TEXT("ABaseAIController::ReleaseTargetTicket: %s"), *Target->GetActorLabel());
	ABaseCharacter* CharacterTarget = Cast<ABaseCharacter>(Target);
	if (CharacterTarget != nullptr)
	{
		CharacterTarget->ReleaseTicket(this);
		GetBlackboardComponent()->SetValueAsBool(HasTicketKeyName, false);
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

void ABaseAIController::OnCharacterDeath(ACharacter* DeadCharacter, AController* KillerController)
{
	ReleaseTargetTicket(GetCurrentTarget());
}

void ABaseAIController::OnChangeTarget(AActor* OldTarget, AActor* NewTarget)
{
	ReleaseTargetTicket(OldTarget);
}

AActor* ABaseAIController::GetCurrentTarget() const
{
	if (AITargetComponent == nullptr)
	{
		return nullptr;
	}

	return AITargetComponent->GetCurrentTarget();
}

void ABaseAIController::StartBehaviour()
{
	RunBehaviorTree(BTAsset);
	if (AITargetComponent != nullptr)
	{
		AITargetComponent->SetBlackboardComponent(Blackboard);
	}
}
