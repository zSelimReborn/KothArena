// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/TicketComponent.h"

struct FRemoveClaimersInvalid
{
	bool operator()(const TWeakObjectPtr<const AController> Controller) const
	{
		return !Controller.IsValid();
	}
};

// Sets default values for this component's properties
UTicketComponent::UTicketComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UTicketComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UTicketComponent::OnRegister()
{
	Super::OnRegister();

	TicketsAvailable = MaxTickets;
}


// Called every frame
void UTicketComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	CheckForInvalidClaimers(DeltaTime);
}

bool UTicketComponent::ClaimTicket(const AController* Controller)
{
	if (Controller == nullptr)
	{
		return false;
	}

	if (Claimers.Contains(Controller))
	{
		return true;
	}

	// TODO: Should this be a "critical" section?
	if (TicketsAvailable <= 0)
	{
		return false;
	}

	Claimers.Add(Controller);
	TicketsAvailable = FMath::Max(0, TicketsAvailable - 1);
	
	return true;
}

void UTicketComponent::ReleaseTicket(const AController* Controller)
{
	if (Controller == nullptr)
	{
		return;
	}

	if (!Claimers.Contains(Controller))
	{
		return;
	}

	Claimers.Remove(Controller);
	TicketsAvailable = FMath::Min(MaxTickets, TicketsAvailable + 1);
}

void UTicketComponent::CheckForInvalidClaimers(const float DeltaTime)
{
	CurrentTimeCheckInvalidClaimers += DeltaTime;
	if (CurrentTimeCheckInvalidClaimers >= SecondsToCheckInvalidClaimers)
	{
		CurrentTimeCheckInvalidClaimers = 0.f;

		if (!Claimers.IsEmpty())
		{
			//int32 NewTickets = Claimers.RemoveAll([&](const TWeakObjectPtr<const AController> Claimer) { return !Claimer.IsValid(); });
			int32 NewTickets = Claimers.RemoveAll(FRemoveClaimersInvalid());
			TicketsAvailable = FMath::Min(MaxTickets, TicketsAvailable + NewTickets);
		}
	}
}

