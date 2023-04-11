// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseSpawner.generated.h"

class ABaseItem;

USTRUCT()
struct FClassProbabilityMapping
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<ABaseItem> ItemClass;
	
	UPROPERTY(EditAnywhere, meta=(ClampMin=0.f, ClampMax=1.f))
	float Probability;
};

UCLASS()
class KOTHARENA_API ABaseSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseSpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void SortClassProbability();
	
	TSubclassOf<ABaseItem> ComputeClassToSpawn() const;

	void CheckEmptyItem();
	
	virtual void SpawnItem();

// Callbacks
protected:
	UFUNCTION()
	void StartSpawnTimer();

	UFUNCTION()
	void OnItemPicked(AActor* ItemUsed, AActor* InstigatorActor);
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

// Components
protected:
	UPROPERTY(VisibleAnywhere, NoClear)
	TObjectPtr<USceneComponent> DefaultSceneComponent;

	UPROPERTY(VisibleAnywhere, NoClear)
	TObjectPtr<UStaticMeshComponent> StandMeshComponent;

	UPROPERTY(VisibleAnywhere, NoClear)
	TObjectPtr<USceneComponent> SpawnLocationComponent;

// Properties
protected:
	UPROPERTY(EditAnywhere, Category="Spawner")
	float MinTimeToSpawn = 5.f;

	UPROPERTY(EditAnywhere, Category="Spawner")
	float MaxTimeToSpawn = 20.f;

	UPROPERTY(EditAnywhere, Category="Spawner")
	float TimeToCheckEmptyItem = 5.f;

	UPROPERTY(EditAnywhere, Category="Spawner")
	bool bSpawnOnBeginPlay = true;

	UPROPERTY()
	bool bIsWaitingToSpawn = false;

	UPROPERTY(EditAnywhere, Category="Spawner")
	TSubclassOf<ABaseItem> DefaultItemClass;

	UPROPERTY(EditAnywhere, Category="Spawner")
	TArray<FClassProbabilityMapping> ItemToSpawnClasses;
	
	UPROPERTY(Transient, VisibleAnywhere)
	TWeakObjectPtr<ABaseItem> ItemRef;

	FTimerHandle CheckEmptyItemHandle;
};
