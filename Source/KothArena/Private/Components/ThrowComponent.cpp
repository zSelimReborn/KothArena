// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/ThrowComponent.h"

#include "Characters/BaseCharacter.h"
#include "Gameplay/Throwable/BaseThrowable.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/GameplayStaticsTypes.h"
#include "Net/UnrealNetwork.h"
#include "Utils/PlayerUtils.h"

static TAutoConsoleVariable<bool> CVarDebugThrowable(
	TEXT("KothArena.Throwable.DebugTrajectory"),
	false,
	TEXT("Show debug info about throwable trajectory."),
	ECVF_Default
);

// Sets default values for this component's properties
UThrowComponent::UThrowComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}

// Called when the game starts
void UThrowComponent::BeginPlay()
{
	Super::BeginPlay();
	BaseCharacterRef = Cast<ABaseCharacter>(GetOwner());
}

FVector UThrowComponent::ComputeLaunchVelocity(const APlayerController* PlayerController, const FVector& StartLocation) const
{
	FVector CenterLocation, CenterDirection;
	UPlayerUtils::ComputeScreenCenterAndDirection(PlayerController, CenterLocation, CenterDirection);

	CenterDirection.Z *= ZValueModifierPercentage;
	const FVector CenterDestination = CenterLocation + CenterDirection * CurrentMagnitude;
	FVector ToCenter = CenterDestination - StartLocation;
	ToCenter.Normalize();

	const FVector LaunchVelocity = ToCenter * CurrentMagnitude;

	if (CVarDebugThrowable->GetBool())
	{
		DrawDebugPoint(GetWorld(), CenterDestination, 5.f, FColor::Red, false, 0.1f);
		DrawDebugDirectionalArrow(GetWorld(), StartLocation, CenterDestination, 5.f, FColor::Blue, false, 0.1f);
	}

	return LaunchVelocity;
}

FVector UThrowComponent::GetHandLocation() const
{
	FVector HandLocation = GetOwner()->GetActorLocation();
	if (BaseCharacterRef != nullptr)
	{
		HandLocation = BaseCharacterRef->GetMesh()->GetSocketLocation(BaseCharacterRef->GetThrowableSocketName());
	}

	return HandLocation;
}

// Called every frame
void UThrowComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateCharge(DeltaTime);
	UpdateTrajectory();
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
	if (!GetOwner()->HasAuthority())
	{
		return;
	}
	
	if (ActionState == EThrowActionState::Charge)
	{
		CurrentSpeedAccumulator = FMath::Clamp(CurrentSpeedAccumulator + DeltaTime, 0.f, SpeedToReachMaxMagnitude);
		const float Ratio = FMath::Clamp(CurrentSpeedAccumulator / SpeedToReachMaxMagnitude, 0.f, 1.f);
		CurrentMagnitude = FMath::Lerp(MinThrowMagnitude, MaxThrowMagnitude, Ratio);
	}
}

void UThrowComponent::UpdateTrajectory()
{
	if (!bShowTrajectory)
	{
		return;
	}
	
	if (!BaseCharacterRef || !BaseCharacterRef->IsLocallyControlled())
	{
		return;
	}
	
	if (ActionState != EThrowActionState::Charge)
	{
		return;
	}
	
	FVector StartLocation = GetHandLocation();
	FPredictProjectilePathParams TrajectoryParams;
	FPredictProjectilePathResult TrajectoryResult;
	
	const FVector LaunchVelocity = ComputeLaunchVelocity(GetPlayerController(), StartLocation);
	
	TrajectoryParams.LaunchVelocity = LaunchVelocity;
	TrajectoryParams.StartLocation = StartLocation;
	TrajectoryParams.bTraceWithCollision = true;
	TrajectoryParams.bTraceWithChannel = true;
	TrajectoryParams.TraceChannel = ECollisionChannel::ECC_Visibility;
	TrajectoryParams.ActorsToIgnore.Add(GetOwner());
	TrajectoryParams.ProjectileRadius = 10.f;
	bool bHit = UGameplayStatics::PredictProjectilePath(GetWorld(), TrajectoryParams, TrajectoryResult);

	FVector LastPosition = StartLocation;
	for (const FPredictProjectilePathPointData& Point : TrajectoryResult.PathData)
	{
		DrawDebugLine(GetWorld(), LastPosition, Point.Location, FColor::Black, false, 0.1f);
		LastPosition = Point.Location;
	}

	const AActor* HitActor = TrajectoryResult.HitResult.GetActor();
	if (bHit && HitActor && HitActor->IsA(ABaseCharacter::StaticClass()))
	{
		DrawDebugSphere(GetWorld(), TrajectoryResult.LastTraceDestination.Location, 10.f, 10, FColor::Red, false, 0.1f);
	}

	if (CVarDebugThrowable->GetBool())
	{
		UE_LOG(LogTemp, Error, TEXT("StartLocation: %s"), *StartLocation.ToString());
		DrawDebugPoint(GetWorld(), StartLocation, 10.f, FColor::Cyan, false, 0.1f);
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
		const FVector HandLocation = GetHandLocation();
		const FVector LaunchVelocity = ComputeLaunchVelocity(GetPlayerController(), HandLocation);
		CurrentThrowable->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		CurrentThrowable->Launch(LaunchVelocity);
		
		CurrentThrowable = nullptr;
		ThrowableInventory--;
		OnChangeQuantityDelegate.Broadcast(ThrowableInventory);
		
		ActionState = EThrowActionState::Idle;
		CurrentSpeedAccumulator = 0.f;
		CurrentMagnitude = MinThrowMagnitude;
	}
}

void UThrowComponent::AddQuantity(const int32 Quantity)
{
	if (!GetOwner()->HasAuthority())
	{
		return;
	}
	
	ThrowableInventory += Quantity;
	OnChangeQuantityDelegate.Broadcast(ThrowableInventory);
}

void UThrowComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UThrowComponent, ThrowableClass);
	DOREPLIFETIME(UThrowComponent, ThrowableInventory);
	DOREPLIFETIME(UThrowComponent, ActionState);
	DOREPLIFETIME(UThrowComponent, CurrentMagnitude);
}

void UThrowComponent::OnRep_ThrowableClass()
{
	OnChangeThrowableDelegate.Broadcast(ThrowableClass, ThrowableInventory);
}

void UThrowComponent::OnRep_ThrowableInventory()
{
	OnChangeQuantityDelegate.Broadcast(ThrowableInventory);
}

void UThrowComponent::ChangeThrowable(const TSubclassOf<ABaseThrowable> NewThrowableClass, const int32 Quantity)
{
	if (!GetOwner()->HasAuthority())
	{
		return;
	}
	
	if (ThrowableClass == NewThrowableClass)
	{
		AddQuantity(Quantity);
	}
	else
	{
		ThrowableClass = NewThrowableClass;
		ThrowableInventory = Quantity;
		OnChangeThrowableDelegate.Broadcast(NewThrowableClass, Quantity);
	}
}

