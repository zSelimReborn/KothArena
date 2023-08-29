// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_Shoot.generated.h"

class ABaseCharacter;

/**
 * 
 */
UCLASS()
class KOTHARENA_API UBTTask_Shoot : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_Shoot(const FObjectInitializer& ObjectInitializer);
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
protected:
	void IncrementBulletShot(UBehaviorTreeComponent& OwnerComp) const;
	void SetBulletShot(UBehaviorTreeComponent& OwnerComp, const int32 NewBulletShot) const;
	bool StartAutomaticFire(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, ABaseCharacter* Character);

// Callbacks
protected:
	UFUNCTION()
	void OnAutomaticWeaponShot(const FHitResult& ShotResult, const FVector& EndShotLocation);
	
protected:
	UPROPERTY(EditAnywhere, Category="Attack")
	FName BlackboardKeyTargetActor = NAME_None;

	UPROPERTY(EditAnywhere, Category="Attack")
	FName BlackboardKeyNumOfBulletShot = NAME_None;

	UPROPERTY(EditAnywhere, Category="Attack|Automatic Fire")
	float AutomaticFireHoldTime = 1.f;

	UPROPERTY(EditAnywhere, Category="Attack|Automatic Fire")
	float AutomaticFireRandomDeviation = 0.2f;
	
	UPROPERTY(Transient)
	int32 AutomaticBulletShot = 0;
};
