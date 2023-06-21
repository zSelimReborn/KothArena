// Fill out your copyright notice in the Description page of Project Settings.


#include "Utils/Map/Spawner.h"

#include "Kismet/GameplayStatics.h"

// Sets default values
ASpawner::ASpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ASpawner::BeginPlay()
{
	Super::BeginPlay();
	
}

AActor* ASpawner::SpawnActor()
{
	if (!HasAuthority())
	{
		return nullptr;
	}
	
	return GetWorld()->SpawnActorDeferred<AActor>(ClassToSpawn, GetActorTransform());
}

void ASpawner::FinishSpawning(AActor* Actor)
{
	if (!HasAuthority())
	{
		return;
	}
	
	UGameplayStatics::FinishSpawningActor(Actor, GetActorTransform());
}

