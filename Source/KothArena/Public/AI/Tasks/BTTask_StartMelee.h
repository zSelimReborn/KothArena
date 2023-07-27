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
	bool TargetIsVisible(const APawn* ControlledPawn, const AActor* Target) const;
	static void RotateToTarget(APawn* ControlledPawn, const AActor* Target);

	void ApplyDamage(AActor* HitActor, AController* Instigator, AActor* Causer) const;
	void PushActorAway(AActor* HitActor, const APawn* ControlledPawn) const;
	void SpawnHitParticle(const FVector& Location, const FRotator& Rotation) const;

// Properties
protected:
	UPROPERTY(EditAnywhere, Category="Attack")
	float Range = 100.f;

	UPROPERTY(EditAnywhere, Category="Trace")
	float SphereRadius = 25.f;

	UPROPERTY(EditAnywhere, Category="Trace")
	float VisibilityCapsuleRadius = 34.f;

	UPROPERTY(EditAnywhere, Category="Trace")
	float VisibilityCapsuleHalfHeight = 88.f;

	UPROPERTY(EditAnywhere, Category="Attack|Damage")
	float MinDamage = 10.f;

	UPROPERTY(EditAnywhere, Category="Attack|Damage")
	float MaxDamage = 25.f;

	UPROPERTY(EditAnywhere, Category="Attack|Damage")
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(EditAnywhere, Category="Attack|Push")
	bool bPushTargetAway = true;

	UPROPERTY(EditAnywhere, Category="Attack|Push")
	float PushForce = 2000.f;

	UPROPERTY(EditAnywhere, Category="Attack|FX")
	TObjectPtr<UParticleSystem> HitParticle;

	UPROPERTY(EditAnywhere, Category="Attack|Rotate")
	FName BlackboardKeyTargetActor = NAME_None;
};
