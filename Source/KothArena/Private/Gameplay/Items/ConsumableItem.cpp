// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Items/ConsumableItem.h"

#include "Components/SphereComponent.h"
#include "GameFramework/RotatingMovementComponent.h"

AConsumableItem::AConsumableItem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
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
	
	TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &AConsumableItem::OnBeginOverlap);
	RotatingMovementComponent->SetUpdatedComponent(BaseMeshComponent);
}

void AConsumableItem::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!bConsumeOnOverlap)
	{
		return;
	}
	
	if (ConsumeItem(OtherActor))
	{
		ItemUsedDelegate.Broadcast(this, OtherActor);
		if (bDestroyOnConsume)
		{
			Destroy();
		}
	}
}

void AConsumableItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AConsumableItem::ConsumeItem(AActor* InstigatorActor)
{
	// Override in children classes
	return false;
}
