// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseThrowable.generated.h"

class UProjectileMovementComponent;
class UCapsuleComponent;
class UParticleSystem;

UCLASS()
class KOTHARENA_API ABaseThrowable : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseThrowable();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void StartDetonateTimer();

	void SpawnExplodeParticle() const;

	UFUNCTION(BlueprintCallable)
	void ApplyPointDamage(AActor* OtherActor);

	UFUNCTION(BlueprintCallable)
	void ApplyRadiusDamage();

// Components interface
public:
	UFUNCTION(BlueprintCallable)
	void Launch(const FVector& Direction);

	UFUNCTION(BlueprintCallable)
	void Explode();

	UFUNCTION(BlueprintCallable)
	void Stick(AActor* ActorToStick, UPrimitiveComponent* ComponentToStick, const FName& BoneName);

// Events
public:
	UFUNCTION(BlueprintImplementableEvent)
	void OnHitSomethingEvent(AActor* HitActor, UPrimitiveComponent* HitComponent);

	UFUNCTION(BlueprintImplementableEvent)
	void OnEndDetonateTimerEvent();

// Callbacks
protected:
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void FireDetonateEvent();

// Components
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, NoClear)
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, NoClear)
	TObjectPtr<UCapsuleComponent> TriggerVolume;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, NoClear)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent;

// Properties
protected:
	UPROPERTY(EditAnywhere, Category="Throwable")
	float BaseMagnitude = 20.f;

	UPROPERTY(EditAnywhere, Category="Detonate")
	bool bShouldDetonate = false;
	
	UPROPERTY(EditAnywhere, Category="Detonate")
	float TimeToDetonate = 5.f;

	UPROPERTY(EditAnywhere, Category="Detonate")
	float DetonateRadius = 200.f;

	UPROPERTY(EditAnywhere, Category="Detonate")
	TObjectPtr<UParticleSystem> ExplodeParticle;

	UPROPERTY(EditAnywhere, Category="Damage")
	float BaseDamage = 50.f;

	UPROPERTY(EditAnywhere, Category="Damage")
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(Transient)
	TWeakObjectPtr<AController> OwnerController;
	
	FTimerHandle DetonateTimerHandle;
};
