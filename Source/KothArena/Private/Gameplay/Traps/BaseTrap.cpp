// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Traps/BaseTrap.h"

#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABaseTrap::ABaseTrap()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	SetReplicateMovement(true);

	BaseMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base Mesh Component"));
	SetRootComponent(BaseMeshComponent);
}

// Called when the game starts or when spawned
void ABaseTrap::BeginPlay()
{
	Super::BeginPlay();
}

void ABaseTrap::ApplyDamage(AActor* ActorToDamage, const float& Damage)
{
	if (ActorToDamage)
	{
		UGameplayStatics::ApplyDamage(ActorToDamage, Damage, nullptr, this, BaseDamageType);
	}
}

void ABaseTrap::PushActorAway(AActor* Actor)
{
	if (Actor)
	{
		ACharacter* Character = Cast<ACharacter>(Actor);
		if (Character)
		{
			const FVector LaunchVector = PushForce;
			Character->LaunchCharacter(LaunchVector, false, false);
		}
	}
}

// Called every frame
void ABaseTrap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
