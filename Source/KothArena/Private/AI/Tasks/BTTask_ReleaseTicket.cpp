// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/BTTask_ReleaseTicket.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/BaseCharacter.h"
#include "Controllers/BaseAIController.h"

UBTTask_ReleaseTicket::UBTTask_ReleaseTicket(const FObjectInitializer& ObjectInitializer)
{
	NodeName = TEXT("Release Target Ticket");

	BlackboardKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_ReleaseTicket, BlackboardKey));
}

EBTNodeResult::Type UBTTask_ReleaseTicket::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	OwnerComp.GetBlackboardComponent()->SetValueAsBool(GetSelectedBlackboardKey(), false);

	const AAIController* OwnerController = OwnerComp.GetAIOwner();
	if (OwnerController != nullptr)
	{
		const ABaseAIController* AIController = Cast<ABaseAIController>(OwnerController);
		if (AIController != nullptr)
		{
			ABaseCharacter* Target = Cast<ABaseCharacter>(AIController->GetCurrentTarget());
			if (Target != nullptr)
			{
				Target->ReleaseTicket(AIController);
			}
		}
	}

	return EBTNodeResult::Succeeded;
}
