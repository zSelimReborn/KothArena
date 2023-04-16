// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/BTTask_StartMelee.h"

EBTNodeResult::Type UBTTask_StartMelee::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UE_LOG(LogTemp, Display, TEXT("Starting Melee Attack!"));
	return EBTNodeResult::Succeeded;
}
