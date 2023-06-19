// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SearchItemComponent.h"

#include "Utils/MathUtils.h"
#include "Utils/PlayerUtils.h"

// Sets default values for this component's properties
USearchItemComponent::USearchItemComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void USearchItemComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerPawn = Cast<APawn>(GetOwner());
}

void USearchItemComponent::SearchForItems()
{
	if (bShouldSearchForItems && OwnerPawn && OwnerPawn->GetController() && OwnerPawn->IsLocallyControlled())
	{
		FVector CameraLocation, CameraDirection;
		if (!UPlayerUtils::ComputeScreenCenterAndDirection(OwnerPawn->GetController<APlayerController>(), CameraLocation, CameraDirection))
		{
			return;
		}

		const FVector CharacterLocation = OwnerPawn->GetActorLocation();
		const FVector CharacterForward = OwnerPawn->GetActorForwardVector();

		const FVector EndTrace = CameraLocation + CameraDirection * SearchTraceLength;

		if (UMathUtils::GetAngleBetweenVectors(CameraDirection, CharacterForward) > SearchMaxFOV)
		{
			if (ItemFoundRef)
			{
				ItemLostDelegate.Broadcast(ItemFoundRef);
				ItemFoundRef = nullptr;
			}
			return;
		}

		FCollisionShape Sphere = FCollisionShape::MakeSphere(SphereSearchRadius);

		FCollisionQueryParams QueryParams{TEXT("KOTHArena::SearchItemComponent")};
		QueryParams.AddIgnoredActor(OwnerPawn);
		
		FHitResult TraceResult;
		const bool bHitSomething = GetWorld()->SweepSingleByChannel(
			TraceResult,
			CharacterLocation,
			EndTrace,
			FQuat::Identity,
			ECollisionChannel::ECC_WorldDynamic,
			Sphere,
			QueryParams
		);

		AActor* FoundItem = (TraceResult.GetActor());
		if (bHitSomething && FoundItem && FoundItem->IsA(ActorClassToSearch))
		{
			if (FoundItem != ItemFoundRef)
			{
				NewItemFoundDelegate.Broadcast(TraceResult, FoundItem);
				ItemFoundRef = FoundItem;
			}
		}
		else
		{
			ItemLostDelegate.Broadcast(ItemFoundRef);
			ItemFoundRef = nullptr;
		}
	}

}


// Called every frame
void USearchItemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	SearchForItems();
}

