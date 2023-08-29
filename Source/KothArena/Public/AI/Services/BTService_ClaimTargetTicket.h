// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_ClaimTargetTicket.generated.h"

/**
 * 
 */
UCLASS()
class KOTHARENA_API UBTService_ClaimTargetTicket : public UBTService_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTService_ClaimTargetTicket(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
