// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_ReleaseTicket.generated.h"

/**
 * 
 */
UCLASS()
class KOTHARENA_API UBTTask_ReleaseTicket : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTTask_ReleaseTicket(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
