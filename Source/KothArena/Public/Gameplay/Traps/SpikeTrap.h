// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/Traps/BaseTrap.h"
#include "SpikeTrap.generated.h"

class UBoxComponent;

UENUM()
enum class ESpikeStatus
{
	Shown,
	Showing,
	Closing,
	Closed
};

/**
 * 
 */
UCLASS()
class KOTHARENA_API ASpikeTrap : public ABaseTrap
{
	GENERATED_BODY()

public:
	ASpikeTrap();

	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void StartTimerToCloseSpikes();
	void StartTimerToShowSpikes();
	FVector ComputeNextRelativeLocation();
	void UpdateSpikes(const float&);

// Callbacks
protected:
	void SetSpikesToClose();
	void SetSpikesToShow();

	UFUNCTION()
	void OnSpikeHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
// Net functions
protected:
	UFUNCTION()
	void OnRep_ServerAccumulatorTime();

	UFUNCTION(Server, Reliable)
	void SynchronizeAccumulator();

// Public interface
public:
	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetTimeToShow(const float NewTimeToShow) { TimeToShow = NewTimeToShow; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetTimeToClose(const float NewTimeToClose) { TimeToClose = NewTimeToClose; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetTimeToResetSpikesAfterShowing(const float NewTime) { TimeToResetSpikesAfterShowing = NewTime; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetTimeToShowSpikesAfterClosing(const float NewTime) { TimeToShowSpikesAfterClosing = NewTime; }


// Components
protected:
	UPROPERTY(VisibleAnywhere, NoClear)
	TObjectPtr<UStaticMeshComponent> SpikeMeshComponent;

	UPROPERTY(VisibleAnywhere, NoClear)
	TObjectPtr<UBoxComponent> TriggerVolume;

// Properties
protected:
	UPROPERTY(EditAnywhere, Category="Spike")
	float ZValueSpikes = 25.f;

	UPROPERTY(Transient, VisibleAnywhere, Category="Spike")
	float InitialZValueSpikes = -25.f;
	
	UPROPERTY(EditAnywhere, Category="Spike")
	float TimeToShow = 1.f;

	UPROPERTY(EditAnywhere, Category="Spike")
	float TimeToClose = 1.f;

	UPROPERTY(EditAnywhere, Category="Spike")
	float TimeToResetSpikesAfterShowing = 2.5;

	UPROPERTY(EditAnywhere, Category="Spike")
	float TimeToShowSpikesAfterClosing = 2.5;

	UPROPERTY(EditAnywhere, Category="Spike")
	FRuntimeFloatCurve ShowSpikesCurve;

	UPROPERTY(EditAnywhere, Category="Spike")
	FRuntimeFloatCurve CloseSpikesCurve;

	UPROPERTY(VisibleAnywhere, Category="Spike")
	float CurrentTimeAccumulator = 0.f;

	UPROPERTY(VisibleAnywhere, Category="Spike|Network", ReplicatedUsing=OnRep_ServerAccumulatorTime)
	float ServerTimeAccumulator = 0.f;
	
	UPROPERTY(VisibleAnywhere, Category="Spike", Replicated)
	ESpikeStatus SpikeStatus = ESpikeStatus::Closed;
};
