// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Traps/SpikeTrap.h"

#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"

ASpikeTrap::ASpikeTrap()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	
	SpikeMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Spike Mesh Component"));
	SpikeMeshComponent->SetIsReplicated(true);
	SpikeMeshComponent->SetupAttachment(BaseMeshComponent);

	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger Volume"));
	TriggerVolume->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	TriggerVolume->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECR_Overlap);
	TriggerVolume->SetupAttachment(SpikeMeshComponent);
}

void ASpikeTrap::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &ASpikeTrap::OnSpikeHit);
		InitialZValueSpikes = SpikeMeshComponent->GetRelativeLocation().Z;
		StartTimerToShowSpikes();
	}
}

void ASpikeTrap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
	{
		UpdateSpikes(DeltaTime);
	}
}

void ASpikeTrap::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASpikeTrap, SpikeStatus);
	DOREPLIFETIME(ASpikeTrap, ServerTimeAccumulator);
}

void ASpikeTrap::StartTimerToCloseSpikes()
{
	FTimerHandle ResetSpikeTimerHandle;
	GetWorldTimerManager().SetTimer(ResetSpikeTimerHandle, this, &ASpikeTrap::SetSpikesToClose, TimeToResetSpikesAfterShowing, false);
}

void ASpikeTrap::StartTimerToShowSpikes()
{
	FTimerHandle ShowSpikeTimerHandle;
	GetWorldTimerManager().SetTimer(ShowSpikeTimerHandle, this, &ASpikeTrap::SetSpikesToShow, TimeToShowSpikesAfterClosing, false);
}

FVector ASpikeTrap::ComputeNextRelativeLocation()
{
	FVector NewSpikeLocation = SpikeMeshComponent->GetRelativeLocation();
	
	const float ShowTimeRatio = FMath::Clamp(CurrentTimeAccumulator / TimeToShow, 0.f, 1.f);
	const float ShowCurveValue = ShowSpikesCurve.GetRichCurveConst()->Eval(ShowTimeRatio);
	
	const float CloseTimeRatio = FMath::Clamp(CurrentTimeAccumulator / TimeToClose, 0.f, 1.f);
	const float CloseCurveValue = CloseSpikesCurve.GetRichCurveConst()->Eval(CloseTimeRatio);

	float CurrentZValue = NewSpikeLocation.Z;
	
	switch (SpikeStatus)
	{
	case ESpikeStatus::Showing:
		if (CurrentTimeAccumulator >= TimeToShow)
		{
			SpikeStatus = ESpikeStatus::Shown;
			StartTimerToCloseSpikes();
		}

		CurrentZValue = FMath::Lerp(InitialZValueSpikes, ZValueSpikes, ShowCurveValue); 
		break;
	case ESpikeStatus::Closing:
		if (CurrentTimeAccumulator >= TimeToClose)
		{
			SpikeStatus = ESpikeStatus::Closed;
			StartTimerToShowSpikes();
		}

		CurrentZValue = FMath::Lerp(InitialZValueSpikes, ZValueSpikes, CloseCurveValue); 
		break;
	}

	NewSpikeLocation.Z = CurrentZValue;
	return NewSpikeLocation;
}

void ASpikeTrap::UpdateSpikes(const float& DeltaTime)
{
	if (SpikeStatus == ESpikeStatus::Closing || SpikeStatus == ESpikeStatus::Showing)
	{
		CurrentTimeAccumulator += DeltaTime;
		const FVector NewSpikeLocation = ComputeNextRelativeLocation();
		SpikeMeshComponent->SetRelativeLocation(NewSpikeLocation);
	}
}

void ASpikeTrap::SetSpikesToClose()
{
	SpikeStatus = ESpikeStatus::Closing;
	CurrentTimeAccumulator = 0.f;
}

void ASpikeTrap::SetSpikesToShow()
{
	SpikeStatus = ESpikeStatus::Showing;
	CurrentTimeAccumulator = 0.f;
}

void ASpikeTrap::OnSpikeHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor)
	{
		return;
	}

	ApplyDamage(OtherActor, BaseDamage);
	if (bPushPlayerAfterDamage)
	{
		PushActorAway(OtherActor);
	}
}

void ASpikeTrap::OnRep_ServerAccumulatorTime()
{
	CurrentTimeAccumulator = ServerTimeAccumulator;		
}

void ASpikeTrap::SynchronizeAccumulator_Implementation()
{
	ServerTimeAccumulator = CurrentTimeAccumulator;
}
