// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ReserveSpot.generated.h"

/**
 * 
 */
UCLASS()
class KOTHARENA_API UBTTask_ReserveSpot : public UBTTaskNode
{
	GENERATED_BODY()

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

// Properties
protected:
	UPROPERTY(EditAnywhere, Category="Reservation")
	float Tolerance = 50.f;
	
	UPROPERTY(EditAnywhere, Category="Reservation")
	struct FBlackboardKeySelector SpotLocationBlackboardKey;
};
