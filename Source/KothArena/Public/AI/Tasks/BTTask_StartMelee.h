// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_StartMelee.generated.h"

class UParticleSystem;

/**
 * 
 */
UCLASS()
class KOTHARENA_API UBTTask_StartMelee : public UBTTaskNode
{
	GENERATED_BODY()

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	FHitResult PerformAttack(const AController* OwnerController, const APawn* ControlledPawn) const;

	void ApplyDamage(AActor* HitActor, AController* Instigator, AActor* Causer) const;
	void PushActorAway(AActor* HitActor, const APawn* ControlledPawn) const;
	void SpawnHitParticle(const FVector& Location, const FRotator& Rotation) const;

// Properties
protected:
	UPROPERTY(EditAnywhere, Category="Attack")
	float Range = 100.f;

	UPROPERTY(EditAnywhere, Category="Trace")
	float SphereRadius = 25.f;

	UPROPERTY(EditAnywhere, Category="Attack")
	float MinDamage = 10.f;

	UPROPERTY(EditAnywhere, Category="Attack")
	float MaxDamage = 25.f;

	UPROPERTY(EditAnywhere, Category="Attack")
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(EditAnywhere, Category="Attack")
	bool bPushTargetAway = true;

	UPROPERTY(EditAnywhere, Category="Attack")
	float PushForce = 2000.f;

	UPROPERTY(EditAnywhere, Category="Attack")
	TObjectPtr<UParticleSystem> HitParticle;
};
