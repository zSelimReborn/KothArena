// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/FindPlayerComponent.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UFindPlayerComponent::UFindPlayerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UFindPlayerComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerController = Cast<AController>(GetOwner());
	PossessedPawn = OwnerController->GetPawn();
	PlayerRef = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	BlackboardComponent = GetOwner()->FindComponentByClass<UBlackboardComponent>();

	if (BlackboardComponent && PlayerRef)
	{
		BlackboardComponent->SetValueAsObject(PlayerObjectBlackboardKey, PlayerRef);
	}
}

void UFindPlayerComponent::UpdatePlayerLocation() const
{
	if (BlackboardComponent && PlayerRef && PossessedPawn)
	{
		const FVector PlayerLocation = PlayerRef->GetActorLocation();
		const FVector CurrentLocation = PossessedPawn->GetActorLocation();
		const float DistanceToPlayer = FVector::Dist(CurrentLocation, PlayerLocation);
		
		BlackboardComponent->SetValueAsVector(PlayerLocationBlackboardKey, PlayerLocation);
		BlackboardComponent->SetValueAsFloat(RangeToPlayerBlackboardKey, DistanceToPlayer);
	}
}


// Called every frame
void UFindPlayerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdatePlayerLocation();
}

