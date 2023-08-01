// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/BTTask_StartMelee.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Utils/PlayerUtils.h"

static TAutoConsoleVariable<bool> CVarDebugMeleeAttack(
	TEXT("KothArena.AI.DebugMeleeAttack"),
	false,
	TEXT("Show debug info on melee attack."),
	ECVF_Default
);

EBTNodeResult::Type UBTTask_StartMelee::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* Controller = OwnerComp.GetAIOwner();
	if (!Controller)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTTask_StartMelee::ExecuteTask -> Unable to get AIController."))
		return EBTNodeResult::Failed;
	}

	APawn* ControlledPawn = Controller->GetPawn();
	if (!ControlledPawn)
	{
		UE_LOG(LogTemp, Error, TEXT("UBTTask_StartMelee::ExecuteTask -> Unable to get controlled pawn."))
		return EBTNodeResult::Failed;
	}

	const AActor* Target = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(BlackboardKeyTargetActor));
	UPlayerUtils::RotateToTarget(ControlledPawn, Target);

	if (!TargetIsVisible(ControlledPawn, Target))
	{
		return EBTNodeResult::Failed;
	}
	
	const FHitResult AttackResult = PerformAttack(Controller, ControlledPawn);
	if (AttackResult.bBlockingHit)
	{
		AActor* HitActor = AttackResult.GetActor();
		const FVector HitLocation = AttackResult.Location;
		
		if (HitActor)
		{
			ApplyDamage(HitActor, Controller, ControlledPawn);
			SpawnHitParticle(HitLocation, FRotator::ZeroRotator);
			if (bPushTargetAway)
			{
				PushActorAway(HitActor, ControlledPawn);
			}
		}
	}
	
	return EBTNodeResult::Succeeded;
}

FHitResult UBTTask_StartMelee::PerformAttack(const AController* OwnerController, const APawn* ControlledPawn) const
{
	const FVector PawnLocation = ControlledPawn->GetActorLocation();
	const FVector PawnDirection = ControlledPawn->GetActorForwardVector();
	const FVector PawnRight = ControlledPawn->GetActorRightVector();
	const FVector PawnLeft = -PawnRight;
	
	const FVector MiddlePoint = PawnLocation + (PawnDirection * Range);
	const FVector LeftSphere = (MiddlePoint) + (PawnLeft * Range);
	const FVector RightSphere = (MiddlePoint) + (PawnRight * Range);

	if (CVarDebugMeleeAttack->GetBool())
	{
		DrawDebugSphere(GetWorld(), LeftSphere, SphereRadius, 10, FColor::Red, true);
		DrawDebugSphere(GetWorld(), RightSphere, SphereRadius, 10, FColor::Blue, true);
	}
	
	FCollisionShape SphereTrace = FCollisionShape::MakeSphere(SphereRadius);
	FCollisionQueryParams MeleeParams{TEXT("MeleeAttack")};
	MeleeParams.AddIgnoredActor(ControlledPawn);
	FHitResult MeleeHit;

	GetWorld()->SweepSingleByChannel(
		MeleeHit,
		LeftSphere,
		RightSphere,
		FRotator::ZeroRotator.Quaternion(),
		ECC_Visibility,
		SphereTrace,
		MeleeParams
	);

	return MeleeHit;
}

bool UBTTask_StartMelee::TargetIsVisible(const APawn* ControlledPawn, const AActor* Target) const
{
	if (ControlledPawn == nullptr || Target == nullptr)
	{
		return false;
	}
	
	const FVector PawnLocation = ControlledPawn->GetActorLocation();
	const FVector TargetLocation = Target->GetActorLocation();
	
	FCollisionShape Capsule = FCollisionShape::MakeCapsule(VisibilityCapsuleRadius, VisibilityCapsuleHalfHeight);

	if (CVarDebugMeleeAttack->GetBool())
	{
		DrawDebugCapsule(GetWorld(), PawnLocation, VisibilityCapsuleHalfHeight, VisibilityCapsuleRadius, FRotator::ZeroRotator.Quaternion(), FColor::Blue, true);
		DrawDebugCapsule(GetWorld(), TargetLocation, VisibilityCapsuleHalfHeight, VisibilityCapsuleRadius, FRotator::ZeroRotator.Quaternion(), FColor::Red, true);
	}

	FCollisionQueryParams QueryParams{TEXT("MeleeVisiblityCheck")};
	QueryParams.AddIgnoredActor(ControlledPawn);
	QueryParams.AddIgnoredActor(Target);
	
	FHitResult SomethingBlocking;
	const bool bHit = GetWorld()->SweepSingleByChannel(
		SomethingBlocking,
		PawnLocation,
		TargetLocation,
		FRotator::ZeroRotator.Quaternion(),
		ECC_Visibility,
		Capsule,
		QueryParams
	);

	// Since we're ignoring ControlledPawn and Target if bHit is true there's something blocking the view
	return !bHit;
}

void UBTTask_StartMelee::ApplyDamage(AActor* HitActor, AController* Instigator, AActor* Causer) const
{
	const float Damage = FMath::RandRange(MinDamage, MaxDamage);
	UGameplayStatics::ApplyDamage(HitActor, Damage, Instigator, Causer, DamageType);
}

void UBTTask_StartMelee::PushActorAway(AActor* HitActor, const APawn* ControlledPawn) const
{
	const FVector PawnDirection = ControlledPawn->GetActorForwardVector();
	ACharacter* HitCharacter = Cast<ACharacter>(HitActor);
	
	if (HitCharacter)
	{
		HitCharacter->GetCharacterMovement()->AddImpulse(PawnDirection * PushForce, true);
	}
}

void UBTTask_StartMelee::SpawnHitParticle(const FVector& Location, const FRotator& Rotation) const
{
	if (HitParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, Location, Rotation);
	}
}
