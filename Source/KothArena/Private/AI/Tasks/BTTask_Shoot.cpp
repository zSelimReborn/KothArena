// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/BTTask_Shoot.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/BaseCharacter.h"
#include "Utils/PlayerUtils.h"

EBTNodeResult::Type UBTTask_Shoot::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const AAIController* OwnerController = OwnerComp.GetAIOwner();
	if (OwnerController == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	ABaseCharacter* Character = Cast<ABaseCharacter>(OwnerController->GetCharacter());
	if (Character == nullptr)
	{
		return EBTNodeResult::Failed;
	}
	
	const AActor* Target = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(BlackboardKeyTargetActor));
	if (Target == nullptr)
	{
		return EBTNodeResult::Failed;
	}
	
	UPlayerUtils::RotateToTarget(Character, Target);

	if (Character->ShouldReload())
	{
		Character->RequestReloadCurrentWeapon();
	}
	
	Character->RequestWeaponPullTrigger();
	Character->RequestWeaponReleaseTrigger();
	
	return EBTNodeResult::Succeeded;
}
