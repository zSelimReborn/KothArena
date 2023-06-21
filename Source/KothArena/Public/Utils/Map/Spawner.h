// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Spawner.generated.h"

UCLASS(meta=(ToolTip="Spawner for level-objects on authority"))
class KOTHARENA_API ASpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

// Public interface
public:
	UFUNCTION(BlueprintCallable)
	AActor* SpawnActor();

	UFUNCTION(BlueprintCallable)
	void FinishSpawning(AActor* Actor);
	
	
// Properties
protected:
	UPROPERTY(EditAnywhere, Category="Spawn")
	TSubclassOf<AActor> ClassToSpawn;
};
