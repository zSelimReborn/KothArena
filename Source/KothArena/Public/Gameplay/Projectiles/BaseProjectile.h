// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseProjectile.generated.h"

class UParticleSystemComponent;
class UProjectileMovementComponent;
class UCapsuleComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnProjectileHitDelegate, AActor*, ProjectileInstigator, AActor*, ActorHit, const FHitResult&, HitResult);

UCLASS()
class KOTHARENA_API ABaseProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

// Callbacks
protected:
	UFUNCTION()
	void OnProjectileHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	FOnProjectileHitDelegate& OnProjectileHit() { return ProjectileHitDelegate; }
	void Fire(const FVector& Direction);

	FORCEINLINE UCapsuleComponent* GetTriggerVolume() { return TriggerVolume; }

// Components
protected:
	UPROPERTY(VisibleAnywhere, NoClear)
	TObjectPtr<USceneComponent> DefaultSceneComponent;

	UPROPERTY(VisibleAnywhere, NoClear)
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(VisibleAnywhere, NoClear)
	TObjectPtr<UCapsuleComponent> TriggerVolume;

	UPROPERTY(VisibleAnywhere, NoClear)
	TObjectPtr<UParticleSystemComponent> ParticleSystemComponent;

	UPROPERTY(VisibleAnywhere, NoClear)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent;

// Properties
protected:
	UPROPERTY(EditAnywhere, Category="Weapon")
	bool bDestroyOnHit = true;
	
	FOnProjectileHitDelegate ProjectileHitDelegate;
};
