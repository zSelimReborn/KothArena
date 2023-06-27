// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/AimComponent.h"

#include "Camera/CameraActor.h"
#include "Characters/BaseCharacter.h"

// Sets default values for this component's properties
UAimComponent::UAimComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}


// Called when the game starts
void UAimComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UAimComponent::OnRegister()
{
	Super::OnRegister();

	BaseCharacterRef = Cast<ABaseCharacter>(GetOwner());
}

void UAimComponent::SwitchToAim()
{
	if (!BaseCharacterRef || !BaseCharacterRef->IsLocallyControlled())
	{
		return;
	}

	APlayerController* PC = BaseCharacterRef->GetController<APlayerController>();
	if (PC && BaseCharacterRef->GetAimCamera())
	{
		PC->SetViewTargetWithBlend(BaseCharacterRef->GetAimCamera(), TimeToAim, HipFireToAimBlendCurve, HipFireToAimBlendCurveExp, true);
		GetOwner()->GetWorldTimerManager().ClearTimer(CooldownTimer);
	}
}

void UAimComponent::SwitchToHipFire()
{
	if (!BaseCharacterRef || !BaseCharacterRef->IsLocallyControlled())
	{
		return;
	}

	APlayerController* PC = BaseCharacterRef->GetController<APlayerController>();
	if (PC)
	{
		PC->SetViewTargetWithBlend(BaseCharacterRef, TimeToHipFire, AimToHipFireBlendCurve, AimToHipFireBlendCurveExp, true);
		StartCooldown();
		bCanAim = false;
	}
}

void UAimComponent::StartCooldown()
{
	GetOwner()->GetWorldTimerManager().SetTimer(CooldownTimer, this, &UAimComponent::FinishCooldown, TimeToCooldown, false);
}

void UAimComponent::FinishCooldown()
{
	bCanAim = true;
}

// Called every frame
void UAimComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UAimComponent::StartAiming()
{
	if (!bCanAim)
	{
		return;
	}
	
	if (BaseCharacterRef)
	{
		BaseCharacterRef->SetAimingState();
		SwitchToAim();
	}
}

void UAimComponent::FinishAiming()
{
	if (BaseCharacterRef)
	{
		BaseCharacterRef->UnsetAimingState();
		SwitchToHipFire();
	}
}

