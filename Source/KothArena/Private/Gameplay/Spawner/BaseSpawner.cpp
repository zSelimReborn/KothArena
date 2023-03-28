// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Spawner/BaseSpawner.h"

#include "Gameplay/Items/BaseItem.h"

static TAutoConsoleVariable<bool> CVarShowSpawnerRandomGenerator(
	TEXT("KothArena.Spawner.ShowRandomGeneratorInfo"),
	false,
	TEXT("Show info about spawner random generator."),
	ECVF_Default);

// Sets default values
ABaseSpawner::ABaseSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	DefaultSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Default Scene Component"));
	SetRootComponent(DefaultSceneComponent);

	StandMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Stand Mesh Component"));
	StandMeshComponent->SetupAttachment(DefaultSceneComponent);

	SpawnLocationComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Spawn Location"));
	SpawnLocationComponent->SetupAttachment(DefaultSceneComponent);
}

// Called when the game starts or when spawned
void ABaseSpawner::BeginPlay()
{
	Super::BeginPlay();

	SortClassProbability();

	if (bSpawnOnBeginPlay)
	{
		SpawnItem();
	}
	else
	{
		StartSpawnTimer();
	}
}

void ABaseSpawner::SortClassProbability()
{
	ItemToSpawnClasses.Sort([](const FClassProbabilityMapping& First, const FClassProbabilityMapping& Second) {
		return First.Probability < First.Probability;
	});
}

TSubclassOf<ABaseItem> ABaseSpawner::ComputeClassToSpawn() const
{
	if (ItemToSpawnClasses.Num() <= 0)
	{
		return DefaultItemClass;	
	}
	
	TSubclassOf<ABaseItem> ItemClass = ItemToSpawnClasses[0].ItemClass;

	const float Random = FMath::RandRange(0.f, 1.f);

	if (CVarShowSpawnerRandomGenerator->GetBool())
	{
		const FString RandomNumberGeneratedText = FString::Printf(TEXT("Random Value: %.2f"), Random);
		DrawDebugString(GetWorld(), SpawnLocationComponent->GetComponentLocation(), RandomNumberGeneratedText, nullptr, FColor::Black);
	}
	for (const FClassProbabilityMapping& ClassProbability : ItemToSpawnClasses)
	{
		if (Random <= ClassProbability.Probability)
		{
			if (CVarShowSpawnerRandomGenerator->GetBool())
			{
				const FString ChosenClassText = FString::Printf(
					TEXT("Chosen Class : %s | Probability: %.2f | Random: %.2f"),
					*ClassProbability.ItemClass->GetName(),
					ClassProbability.Probability,
					Random
				);
				
				const FVector ChosenClassTextLocation = {SpawnLocationComponent->GetComponentLocation().X, SpawnLocationComponent->GetComponentLocation().Y, SpawnLocationComponent->GetComponentLocation().Z - 25.f};
				DrawDebugString(GetWorld(), ChosenClassTextLocation, ChosenClassText, nullptr, FColor::Black);
			}

			return ClassProbability.ItemClass;
		}
	}
	
	return ItemClass;
}

void ABaseSpawner::SpawnItem()
{
	if (ItemRef.IsValid() && ItemRef != nullptr)
	{
		return;
	}

	const TSubclassOf<ABaseItem> ItemClassToSpawn = ComputeClassToSpawn();
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this;
	ABaseItem* NewItem = GetWorld()->SpawnActor<ABaseItem>(
		ItemClassToSpawn,
		SpawnLocationComponent->GetComponentLocation(),
		SpawnLocationComponent->GetComponentRotation(),
		SpawnParameters
	);
	
	if (NewItem)
	{
		ItemRef = NewItem;
		ItemRef->DisableItem();
		ItemRef->OnItemUsed().AddDynamic(this, &ABaseSpawner::OnItemPicked);
		ItemRef->EnableItem();
	}
}

void ABaseSpawner::StartSpawnTimer()
{
	const float SpawnTime = FMath::RandRange(MinTimeToSpawn, MaxTimeToSpawn);
	FTimerHandle SpawnItemHandler;
	GetWorldTimerManager().SetTimer(SpawnItemHandler, this, &ABaseSpawner::SpawnItem, SpawnTime, false);
}

void ABaseSpawner::OnItemPicked(AActor* ItemUsed, AActor* InstigatorActor)
{
	if (ItemRef != ItemUsed)
	{
		return;
	}
	
	if (ItemRef.IsValid())
	{
		ItemRef->OnItemUsed().RemoveDynamic(this, &ABaseSpawner::OnItemPicked);
		ItemRef= nullptr;
		StartSpawnTimer();
		if (CVarShowSpawnerRandomGenerator->GetBool())
		{
			FlushDebugStrings(GetWorld());
		}
	}
}

// Called every frame
void ABaseSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

