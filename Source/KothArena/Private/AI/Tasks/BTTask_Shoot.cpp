// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/BTTask_Shoot.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/BaseCharacter.h"
#include "Components/WeaponFireComponent.h"
#include "Utils/PlayerUtils.h"

UBTTask_Shoot::UBTTask_Shoot(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NodeName = TEXT("Shoot");
	bTickIntervals = true;
	INIT_TASK_NODE_NOTIFY_FLAGS();
}

EBTNodeResult::Type UBTTask_Shoot::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* OwnerController = OwnerComp.GetAIOwner();
	if (OwnerController == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	ABaseCharacter* Character = Cast<ABaseCharacter>(OwnerController->GetCharacter());
	if (Character == nullptr)
	{
		return EBTNodeResult::Failed;
	}
	
	AActor* Target = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(BlackboardKeyTargetActor));
	if (Target == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	const ABaseWeapon* CharacterWeapon = Character->GetCurrentWeapon();
	if (CharacterWeapon == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	OwnerController->SetFocus(Target);
	UPlayerUtils::RotateToTarget(Character, Target, true);

	if (Character->ShouldReload())
	{
		Character->RequestReloadCurrentWeapon();
	}

	EBTNodeResult::Type ShotResult = EBTNodeResult::Succeeded;

	// Branch based on weapon fire type
	switch (CharacterWeapon->GetWeaponFireType())
	{
	case EWeaponFireType::Automatic:
		{
			// Start latent task if auto fire gets started
			if (StartAutomaticFire(OwnerComp, NodeMemory, Character))
			{
				ShotResult = EBTNodeResult::InProgress;
			}
			else
			{
				ShotResult = EBTNodeResult::Failed;
			}
		}
		break;
	case EWeaponFireType::Single:;
	case EWeaponFireType::Burst:;
	case EWeaponFireType::ConeSpread:;
	case EWeaponFireType::Projectile:
	default:
		Character->RequestWeaponPullTrigger();
		Character->RequestWeaponReleaseTrigger();
		IncrementBulletShot(OwnerComp);
	}
	
	return ShotResult;
}

void UBTTask_Shoot::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	
	ensure(GetSpecialNodeMemory<FBTTaskMemory>(NodeMemory)->NextTickRemainingTime <= 0.f);

	// Reached when hold time finished
	// Release trigger and set how many bullets have been shot
	const AAIController* OwnerController = OwnerComp.GetAIOwner();
	if (OwnerController != nullptr)
	{
		ABaseCharacter* Character = Cast<ABaseCharacter>(OwnerController->GetCharacter());
		if (Character != nullptr)
		{
			Character->RequestWeaponReleaseTrigger();
		}
	}

	SetBulletShot(OwnerComp, AutomaticBulletShot);
	AutomaticBulletShot = 0;
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}

void UBTTask_Shoot::IncrementBulletShot(UBehaviorTreeComponent& OwnerComp) const
{
	int32 CurrentBulletShot = OwnerComp.GetBlackboardComponent()->GetValueAsInt(BlackboardKeyNumOfBulletShot);
	CurrentBulletShot += 1;
	OwnerComp.GetBlackboardComponent()->SetValueAsInt(BlackboardKeyNumOfBulletShot, CurrentBulletShot);
}

void UBTTask_Shoot::SetBulletShot(UBehaviorTreeComponent& OwnerComp, const int32 NewBulletShot) const
{
	OwnerComp.GetBlackboardComponent()->SetValueAsInt(BlackboardKeyNumOfBulletShot, NewBulletShot);
}

bool UBTTask_Shoot::StartAutomaticFire(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, ABaseCharacter* Character)
{
	if (Character == nullptr)
	{
		return false; 
	}

	AutomaticBulletShot = OwnerComp.GetBlackboardComponent()->GetValueAsInt(BlackboardKeyNumOfBulletShot);
	
	const float HoldTime = FMath::FRandRange(
	FMath::Max(0.0f, AutomaticFireHoldTime - AutomaticFireRandomDeviation),
	(AutomaticFireHoldTime + AutomaticFireRandomDeviation)
	);
	SetNextTickTime(NodeMemory, HoldTime);

	ABaseWeapon* CurrentWeapon = Character->GetCurrentWeapon();
	if (CurrentWeapon != nullptr)
	{
		// Bind to "Weapon Shot" event
		UWeaponFireComponent* FireComponent = CurrentWeapon->FindComponentByClass<UWeaponFireComponent>();
		if (FireComponent != nullptr)
		{
			if (!FireComponent->OnWeaponShotDelegate().IsAlreadyBound(this, &UBTTask_Shoot::OnAutomaticWeaponShot))
			{
				FireComponent->OnWeaponShotDelegate().AddDynamic(this, &UBTTask_Shoot::OnAutomaticWeaponShot);
			}
		}
	}
	
	Character->RequestWeaponPullTrigger();
	
	return true;
}

void UBTTask_Shoot::OnAutomaticWeaponShot(const FHitResult& ShotResult, const FVector& EndShotLocation)
{
	AutomaticBulletShot += 1;
}
