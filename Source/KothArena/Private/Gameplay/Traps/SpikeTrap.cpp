// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Traps/SpikeTrap.h"

#include "Components/BoxComponent.h"

ASpikeTrap::ASpikeTrap()
{
	SpikeMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Spike Mesh Component"));
	SpikeMeshComponent->SetupAttachment(BaseMeshComponent);

	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger Volume"));
	TriggerVolume->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	TriggerVolume->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECR_Overlap);
	TriggerVolume->SetupAttachment(SpikeMeshComponent);
}

void ASpikeTrap::BeginPlay()
{
	Super::BeginPlay();

	TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &ASpikeTrap::OnSpikeHit);
	InitialZValueSpikes = SpikeMeshComponent->GetRelativeLocation().Z;
	StartTimerToShowSpikes();
}

void ASpikeTrap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateSpikes(DeltaTime);
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

void ASpikeTrap::UpdateSpikes(const float& DeltaTime)
{
	if (SpikeStatus == ESpikeStatus::Closed || SpikeStatus == ESpikeStatus::Shown)
	{
		return;
	}
	
	FVector NewSpikeLocation = SpikeMeshComponent->GetRelativeLocation();

	CurrentTimeAccumulator += DeltaTime;
	
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
	SpikeMeshComponent->SetRelativeLocation(NewSpikeLocation);
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
