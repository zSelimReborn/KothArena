// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SearchItemComponent.h"

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

float USearchItemComponent::GetAngleBetweenVectors(FVector First, FVector Second) const
{
	First.Normalize();
	Second.Normalize();

	const float Cos = FVector::DotProduct(First, Second);
	const float ACos = FMath::Acos(Cos);
	const float Angle = FMath::RadiansToDegrees(ACos);

	return Angle;
}

void USearchItemComponent::SearchForItems()
{
	if (bShouldSearchForItems && OwnerPawn && OwnerPawn->GetController())
	{
		FVector CameraLocation; FRotator CameraRotation;
		OwnerPawn->GetController()->GetPlayerViewPoint(CameraLocation, CameraRotation);

		const FVector CharacterForward = OwnerPawn->GetActorForwardVector();

		const FVector CameraForward = CameraRotation.Vector();
		const FVector EndTrace = CameraLocation + (CameraForward * SearchTraceLength);

		if (GetAngleBetweenVectors(CameraForward, CharacterForward) > SearchMaxFOV)
		{
			if (ItemFoundRef)
			{
				ItemLostDelegate.Broadcast(ItemFoundRef);
				ItemFoundRef = nullptr;
			}
			return;
		}

		FCollisionShape Sphere = FCollisionShape::MakeSphere(100.f);

		FCollisionQueryParams QueryParams{TEXT("KOTHArena::SearchItemComponent")};
		QueryParams.AddIgnoredActor(OwnerPawn);
		FHitResult TraceResult;
		const bool bHitSomething = GetWorld()->SweepSingleByChannel(
			TraceResult,
			CameraLocation,
			EndTrace,
			FQuat::Identity,
			ECollisionChannel::ECC_Visibility,
			Sphere,
			QueryParams
		);

		AActor* FoundItem = (TraceResult.GetActor());
		if (bHitSomething && FoundItem)
		{
			NewItemFoundDelegate.Broadcast(TraceResult, FoundItem);
			ItemFoundRef = FoundItem;
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

