// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseTrap.generated.h"

class UBoxComponent;

UCLASS()
class KOTHARENA_API ABaseTrap : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseTrap();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	virtual void ApplyDamage(AActor* ActorToDamage, const float& Damage);

	virtual void PushActorAway(AActor* Actor);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

// Components
protected:
	UPROPERTY(VisibleAnywhere, NoClear)
	TObjectPtr<UStaticMeshComponent> BaseMeshComponent;

// Properties
protected:
	UPROPERTY(EditAnywhere, Category="Trap")
	float BaseDamage = 5.f;

	UPROPERTY(EditAnywhere, Category="Trap")
	TSubclassOf<UDamageType> BaseDamageType;
	
	UPROPERTY(EditAnywhere, Category="Trap")
	bool bPushPlayerAfterDamage = true;

	UPROPERTY(EditAnywhere, Category="Trap", meta=(ClampMin=0.f, ClampMax=1000.f))
	float PushForce = 800.f;

	UPROPERTY(EditAnywhere, Category="Trap", meta=(ClampMin=0.f, ClampMax=1.f))
	float PushZValue = 0.2f;
};
