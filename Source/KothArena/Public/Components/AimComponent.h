// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AimComponent.generated.h"

class ABaseCharacter;
class UCameraComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KOTHARENA_API UAimComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAimComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void OnRegister() override;
	
	void SwitchToAim();
	void SwitchToHipFire();

	void UpdateAimAssist();

	void StartCooldown();
	
	UFUNCTION()
	void FinishCooldown();

	bool ShouldActivateAimAssist(const AActor* Enemy, const FVector& CameraLocation, const FVector& CameraDirection) const;
	
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

// Component interface
public:
	void StartAiming();
	void FinishAiming();
	FORCEINLINE bool IsAimAssistActive() const { return bAimAssistActive; };
	
// Properties
protected:
	UPROPERTY(EditAnywhere, Category="Camera|Settings")
	TEnumAsByte<EViewTargetBlendFunction> HipFireToAimBlendCurve;

	UPROPERTY(EditAnywhere, Category="Camera|Settings")
	float HipFireToAimBlendCurveExp = 0.f;

	UPROPERTY(EditAnywhere, Category="Camera|Settings")
	TEnumAsByte<EViewTargetBlendFunction> AimToHipFireBlendCurve;

	UPROPERTY(EditAnywhere, Category="Camera|Settings")
	float AimToHipFireBlendCurveExp = 0.f;

	UPROPERTY(EditAnywhere, Category="Camera|Aim")
	float TimeToAim = 0.2f;

	UPROPERTY(EditAnywhere, Category="Camera|Aim")
	float TimeToHipFire = 0.2f;

	UPROPERTY(EditAnywhere, Category="Camera|Aim")
	float TimeToCooldown = 0.2f;

	UPROPERTY(EditAnywhere, Category="Camera|Aim")
	bool bAimAssistEnabled = true;

	UPROPERTY(EditAnywhere, Category="Camera|Aim")
	float AimAssistDistanceThreshold = 5000.f;

	UPROPERTY(EditAnywhere, Category="Camera|Aim")
	float AimAssistDotThreshold = 0.99f;
	
	UPROPERTY(VisibleAnywhere, Transient)
	bool bCanAim = true;

	UPROPERTY(VisibleAnywhere, Transient)
	bool bAimAssistActive = false;

	UPROPERTY(Transient)
	TWeakObjectPtr<const AActor> AimAssistEnemy = nullptr;
	
	UPROPERTY(Transient)
	TObjectPtr<ABaseCharacter> BaseCharacterRef;

	FTimerHandle CooldownTimer;
};
