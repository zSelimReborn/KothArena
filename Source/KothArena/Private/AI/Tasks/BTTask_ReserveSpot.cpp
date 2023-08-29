// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/BTTask_ReserveSpot.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Subsystems/AIReservationSubsystem.h"

EBTNodeResult::Type UBTTask_ReserveSpot::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	const FName SpotLocationKeyName = SpotLocationBlackboardKey.SelectedKeyName;
	const FVector SpotLocation = OwnerComp.GetBlackboardComponent()->GetValueAsVector(SpotLocationKeyName);

	UAIReservationSubsystem* AIReservationSubsystem = GetWorld()->GetSubsystem<UAIReservationSubsystem>();
	if (AIReservationSubsystem == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	const bool bReserved = AIReservationSubsystem->ReserveSpot(Querier, SpotLocation, Tolerance);
	if (!bReserved)
	{
		return EBTNodeResult::Failed;
	}

	return EBTNodeResult::Succeeded;
}
