// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/ThrowComponent.h"

#include "Characters/BaseCharacter.h"
#include "Gameplay/Throwable/BaseThrowable.h"
#include "Utils/PlayerUtils.h"

// Sets default values for this component's properties
UThrowComponent::UThrowComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UThrowComponent::BeginPlay()
{
	Super::BeginPlay();
	BaseCharacterRef = Cast<ABaseCharacter>(GetOwner());
}


// Called every frame
void UThrowComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateCharge(DeltaTime);
}

APlayerController* UThrowComponent::GetPlayerController()
{
	if (PC != nullptr)
	{
		return PC; 
	}

	if (BaseCharacterRef)
	{
		PC = BaseCharacterRef->GetController<APlayerController>();
	}

	return PC;
}

void UThrowComponent::UpdateCharge(const float DeltaTime)
{
	if (ActionState == EThrowActionState::Charge)
	{
		CurrentSpeedAccumulator = FMath::Clamp(CurrentSpeedAccumulator + DeltaTime, 0.f, SpeedToReachMaxMagnitude);
		const float Ratio = FMath::Clamp(CurrentSpeedAccumulator / SpeedToReachMaxMagnitude, 0.f, 1.f);
		CurrentMagnitude = FMath::Lerp(MinThrowMagnitude, MaxThrowMagnitude, Ratio);
	}
}

ABaseThrowable* UThrowComponent::SpawnAndAttachThrowable() const
{
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = GetOwner();
	SpawnParameters.Instigator = BaseCharacterRef;
		
	ABaseThrowable* NewThrowable = GetWorld()->SpawnActor<ABaseThrowable>(
		ThrowableClass,
		GetOwner()->GetActorLocation(),
		GetOwner()->GetActorRotation(),
		SpawnParameters
	);

	if (NewThrowable)
	{
		if (BaseCharacterRef != nullptr)
		{
			NewThrowable->AttachToComponent(BaseCharacterRef->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, BaseCharacterRef->GetThrowableSocketName());
			BaseCharacterRef->MoveIgnoreActorAdd(NewThrowable);
		}
		else
		{
			NewThrowable->AttachToActor(GetOwner(), FAttachmentTransformRules::KeepWorldTransform);
		}
	}

	return NewThrowable;
}

ABaseThrowable* UThrowComponent::StartThrowing()
{
	if (ThrowableInventory <= 0 || !ThrowableClass)
	{
		return nullptr;
	}
	
	if (ActionState == EThrowActionState::Idle)
	{
		ActionState = EThrowActionState::Charge;
		CurrentThrowable = SpawnAndAttachThrowable();
		return CurrentThrowable;
	}

	return nullptr;
}

void UThrowComponent::FinishThrowing()
{
	if (CurrentThrowable != nullptr)
	{
		FVector CenterLocation, CenterDirection;
		UPlayerUtils::ComputeScreenCenterAndDirection(GetPlayerController(), CenterLocation, CenterDirection);
		
		CurrentThrowable->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		CurrentThrowable->Launch(CenterDirection * CurrentMagnitude);
		
		CurrentThrowable = nullptr;
		ThrowableInventory--;
		
		ActionState = EThrowActionState::Idle;
		CurrentSpeedAccumulator = 0.f;
		CurrentMagnitude = MinThrowMagnitude;
	}
}

void UThrowComponent::AddQuantity(const int32 Quantity)
{
	ThrowableInventory += Quantity;
}

void UThrowComponent::ChangeThrowable(const TSubclassOf<ABaseThrowable> NewThrowableClass, const int32 Quantity)
{
	if (ThrowableClass == NewThrowableClass)
	{
		AddQuantity(Quantity);
	}
	else
	{
		ThrowableClass = NewThrowableClass;
		ThrowableInventory = Quantity;
	}
}

