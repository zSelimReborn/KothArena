// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Items/ConsumableItem.h"

#include "Components/SphereComponent.h"
#include "GameFramework/RotatingMovementComponent.h"

AConsumableItem::AConsumableItem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	
	BaseMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base Mesh Component"));
	BaseMeshComponent->SetupAttachment(DefaultSceneComponent);

	TriggerVolume = CreateDefaultSubobject<USphereComponent>(TEXT("Trigger Volume"));
	TriggerVolume->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	TriggerVolume->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECR_Overlap);
	TriggerVolume->SetupAttachment(DefaultSceneComponent);

	RotatingMovementComponent = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("Rotating Movement Component"));
}

void AConsumableItem::BeginPlay()
{
	Super::BeginPlay();

	bIsItemEnabled = bIsItemEnabledOnBeginPlay;

	if (HasAuthority())
	{
		TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &AConsumableItem::OnBeginOverlap);
		TriggerVolume->OnComponentEndOverlap.AddDynamic(this, &AConsumableItem::OnEndOverlap);
	}
	
	RotatingMovementComponent->SetUpdatedComponent(BaseMeshComponent);
}

void AConsumableItem::RequestConsumeItem(AActor* InstigatorActor)
{
	if (ConsumeItem(InstigatorActor))
	{
		MulticastAfterConsumeItem(InstigatorActor);
		if (bDestroyOnConsume)
		{
			Destroy();
		}
	}
}

void AConsumableItem::ServerAfterConsumeItem_Implementation(AActor* InstigatorActor)
{
	MulticastAfterConsumeItem(InstigatorActor);
}

void AConsumableItem::MulticastAfterConsumeItem_Implementation(AActor* InstigatorActor)
{
	OnConsumeItem(this, InstigatorActor);
	ItemUsedDelegate.Broadcast(this, InstigatorActor);
}

void AConsumableItem::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	LastOverlappingActor = OtherActor;
	
	if (!IsItemEnabled())
	{
		return;
	}
	
	if (!bConsumeOnOverlap)
	{
		return;
	}

	RequestConsumeItem(OtherActor);
}

void AConsumableItem::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (LastOverlappingActor.IsValid())
	{
		LastOverlappingActor = nullptr;
	}
}

bool AConsumableItem::ConsumeItem(AActor* InstigatorActor)
{
	// Override in children classes
	return false;
}

void AConsumableItem::EnableItem()
{
	Super::EnableItem();

	if (LastOverlappingActor.IsValid())
	{
		if (TriggerVolume->IsOverlappingActor(LastOverlappingActor.Get()))
		{
			RequestConsumeItem(LastOverlappingActor.Get());
		}
	}
}
