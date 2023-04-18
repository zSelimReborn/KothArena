// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/AITargetComponent.h"

#include "BehaviorTree/BlackboardComponent.h"
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
		BlackboardComponent = OwnerController->FindComponentByClass<UBlackboardComponent>();
	}
}

void UAITargetComponent::SelectTarget(const float DeltaTime)
{
	CurrentTimeUpdateTarget += DeltaTime;
	if (CurrentTimeUpdateTarget >= TimeToUpdateTarget)
	{
		CurrentTimeUpdateTarget = 0.f;

		SelectedTarget = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
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


// Called every frame
void UAITargetComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	SelectTarget(DeltaTime);
	PublishTargetInformation();
}

