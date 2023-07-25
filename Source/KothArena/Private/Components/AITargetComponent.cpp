// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/AITargetComponent.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/BaseCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UAITargetComponent::UAITargetComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAITargetComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerController = Cast<AController>(GetOwner());
	if (OwnerController)
	{
		PossessedPawn = OwnerController->GetPawn();
		CharacterRef = Cast<ABaseCharacter>(PossessedPawn);
		BlackboardComponent = OwnerController->FindComponentByClass<UBlackboardComponent>();
	}
}

void UAITargetComponent::SelectTarget(const float DeltaTime)
{
	CurrentTimeUpdateTarget += DeltaTime;
	if (CurrentTimeUpdateTarget >= TimeToUpdateTarget)
	{
		CurrentTimeUpdateTarget = 0.f;

		AActor* CandidateTarget = FindNearestTarget();
		if (CharacterRef != nullptr)
		{
			AActor* DamageCauserTarget = CharacterRef->GetLastDamageCauser();
			if (CandidateTarget == nullptr)
			{
				CandidateTarget = DamageCauserTarget;
			}
			else if (DamageCauserTarget && CandidateTarget != DamageCauserTarget)
			{
				const float DistanceToNearestTarget = FVector::Dist(CandidateTarget->GetActorLocation(), PossessedPawn->GetActorLocation());
				// Whomever causes me damage has priority on nearest target
				// But only if I'm too far from him
				// If nearest target it's near me enough that I can hit him, I will prefer him
				if (DistanceToNearestTarget > DistanceToHitTarget)
				{
					CandidateTarget = DamageCauserTarget;
				}
			}
		}
		
		SelectedTarget = CandidateTarget;
	}
}

void UAITargetComponent::PublishTargetInformation() const
{
	if (SelectedTarget.IsValid() && PossessedPawn && BlackboardComponent)
	{
		const FVector TargetLocation = SelectedTarget->GetActorLocation();
		const FVector OwnerLocation = PossessedPawn->GetActorLocation();
		const float DistanceToTarget = FVector::Dist(TargetLocation, OwnerLocation);

		BlackboardComponent->SetValueAsObject(TargetObjectBlackboardKey, SelectedTarget.Get());
		BlackboardComponent->SetValueAsVector(TargetLocationBlackboardKey, TargetLocation);
		BlackboardComponent->SetValueAsFloat(TargetDistanceBlackboardKey, DistanceToTarget);
	}
}

AActor* UAITargetComponent::FindNearestTarget() const
{
	if (PossessedPawn == nullptr)
	{
		return nullptr;
	}
	
	AActor* Target = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (Target == nullptr)
	{
		return nullptr;
	}

	float MinDistance = FVector::Dist(PossessedPawn->GetActorLocation(), Target->GetActorLocation());
		
	const int32 NumOfPlayers = UGameplayStatics::GetNumPlayerControllers(GetWorld());
	for (int32 PlayerIndex = 1; PlayerIndex < NumOfPlayers; ++PlayerIndex)
	{
		AActor* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), PlayerIndex);
		if (Player != nullptr)
		{
			const float DistanceToPlayer = FVector::Dist(PossessedPawn->GetActorLocation(), Player->GetActorLocation());
			if (DistanceToPlayer < MinDistance)
			{
				MinDistance = DistanceToPlayer;
				Target = Player;
			}
		}
	}

	return Target;
}


// Called every frame
void UAITargetComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	SelectTarget(DeltaTime);
	PublishTargetInformation();
}

