// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Services/BTService_ClaimTargetTicket.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/BaseCharacter.h"
#include "Controllers/BaseAIController.h"

UBTService_ClaimTargetTicket::UBTService_ClaimTargetTicket(const FObjectInitializer& ObjectInitializer)
{
	NodeName = TEXT("Try claim a ticket for current target");

	bNotifyBecomeRelevant = false;
	bNotifyCeaseRelevant = false;
	
	// accept only bool keys
	// Have or not have a ticket
	BlackboardKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_ClaimTargetTicket, BlackboardKey));
}

void UBTService_ClaimTargetTicket::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	const AAIController* OwnerController = OwnerComp.GetAIOwner();
	if (OwnerController == nullptr)
	{
		return;
	}

	const ABaseAIController* AIController = Cast<ABaseAIController>(OwnerController);
	if (AIController == nullptr)
	{
		return;
	}

	AActor* Target = AIController->GetCurrentTarget();
	if (Target == nullptr)
	{
		return;
	}

	ABaseCharacter* CharacterTarget = Cast<ABaseCharacter>(Target);
	if (CharacterTarget == nullptr)
	{
		return;
	}

	const bool bTicketClaimed = CharacterTarget->ClaimTicket(AIController);
	OwnerComp.GetBlackboardComponent()->SetValueAsBool(GetSelectedBlackboardKey(), bTicketClaimed);
}
