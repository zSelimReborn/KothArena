// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Items/ThrowableItem.h"

#include "Characters/BaseCharacter.h"
#include "Components/WidgetComponent.h"
#include "Gameplay/Throwable/BaseThrowable.h"

AThrowableItem::AThrowableItem()
{
	PrimaryActorTick.bCanEverTick = false;

	PickupThrowableWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Pickup Throwable Widget Component"));
	PickupThrowableWidgetComponent->SetupAttachment(DefaultSceneComponent);
}

void AThrowableItem::BeginPlay()
{
	Super::BeginPlay();

	PickupThrowableWidgetComponent->SetVisibility(false);
}

bool AThrowableItem::ConsumeItem(AActor* InstigatorActor)
{
	Super::ConsumeItem(InstigatorActor);
	if (ABaseCharacter* Character = Cast<ABaseCharacter>(InstigatorActor))
	{
		if (ThrowableClass == Character->GetCurrentThrowable())
		{
			Character->RequestAddThrowableQuantity(Quantity);
			return true;
		}
	}

	return false;
}
