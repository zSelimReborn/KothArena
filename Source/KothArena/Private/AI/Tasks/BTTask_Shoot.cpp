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

	// TODO branch based on current weapon
	Character->RequestWeaponPullTrigger();
	Character->RequestWeaponReleaseTrigger();
	IncrementBulletShot(OwnerComp);
	
	return EBTNodeResult::Succeeded;
}

void UBTTask_Shoot::IncrementBulletShot(UBehaviorTreeComponent& OwnerComp) const
{
	int32 CurrentBulletShot = OwnerComp.GetBlackboardComponent()->GetValueAsInt(BlackboardKeyNumOfBulletShot);
	CurrentBulletShot += 1;
	OwnerComp.GetBlackboardComponent()->SetValueAsInt(BlackboardKeyNumOfBulletShot, CurrentBulletShot);
}
