// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TicketComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KOTHARENA_API UTicketComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTicketComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void OnRegister() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

// Public interface
public:
	bool ClaimTicket(const AController* Controller);
	
	void ReleaseTicket(const AController* Controller);

protected:
	void CheckForInvalidClaimers(const float DeltaTime);
	
// Properties
protected:
	UPROPERTY(EditAnywhere, Category="Tickets")
	float SecondsToCheckInvalidClaimers = 10.f;

	UPROPERTY(Transient)
	float CurrentTimeCheckInvalidClaimers = 0.f;
	
	UPROPERTY(EditAnywhere, Category="Tickets")
	int32 MaxTickets = 3;

	UPROPERTY(Transient, VisibleAnywhere, Category="Tickets")
	int32 TicketsAvailable = 0;

	UPROPERTY(Transient)
	TArray<TWeakObjectPtr<const AController>> Claimers;
};
