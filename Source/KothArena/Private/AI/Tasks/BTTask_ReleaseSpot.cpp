// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/BTTask_ReleaseSpot.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Subsystems/AIReservationSubsystem.h"

EBTNodeResult::Type UBTTask_ReleaseSpot::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const AAIController* OwnerController = OwnerComp.GetAIOwner();
	if (OwnerController == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	AActor* Querier = OwnerController->GetPawn();
	if (Querier == nullptr)
	{
		return EBTNodeResult::Failed;
	}
	
	UAIReservationSubsystem* AIReservationSubsystem = GetWorld()->GetSubsystem<UAIReservationSubsystem>();
	if (AIReservationSubsystem == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	AIReservationSubsystem->ReleaseSpot(Querier);
	return EBTNodeResult::Succeeded;
}
