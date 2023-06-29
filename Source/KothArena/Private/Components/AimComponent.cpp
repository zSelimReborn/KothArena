// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/AimComponent.h"

#include "Camera/CameraActor.h"
#include "Characters/BaseCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Utils/MathUtils.h"
#include "Utils/PlayerUtils.h"

static TAutoConsoleVariable<bool> CVarDebugAimAssist(
	TEXT("KothArena.Aiming.DebugAimAssist"),
	false,
	TEXT("Show debug info on aim assist."),
	ECVF_Default
);

// Sets default values for this component's properties
UAimComponent::UAimComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}


// Called when the game starts
void UAimComponent::BeginPlay()
{
	Super::BeginPlay();
	SetComponentTickEnabled(false);
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
		SetComponentTickEnabled(true);
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
		SetComponentTickEnabled(false);
	}
}

void UAimComponent::UpdateAimAssist()
{
	if (!bAimAssistEnabled || BaseCharacterRef == nullptr)
	{
		return;
	}
	
	FVector PlayerLookLocation, PlayerLookDirection;
	UPlayerUtils::ComputeScreenCenterAndDirection(BaseCharacterRef->GetController<APlayerController>(), PlayerLookLocation, PlayerLookDirection);
	PlayerLookDirection.Normalize();
	
	// If already aiming someone skip everything else. If aim assist got lost, check every other enemy.
	if (bAimAssistActive &&
		AimAssistEnemy != nullptr &&
		ShouldActivateAimAssist(AimAssistEnemy.Get(), PlayerLookLocation, PlayerLookDirection)
	) {
		return;
	}

	bAimAssistActive = false;
	AimAssistEnemy = nullptr;
	
	TArray<AActor*> Enemies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseCharacter::StaticClass(), Enemies);

	if (Enemies.Num() <= 0)
	{
		return;
	}

	for (const AActor* Enemy : Enemies)
	{
		if (ShouldActivateAimAssist(Enemy, PlayerLookLocation, PlayerLookDirection))
		{
			bAimAssistActive = true;
			AimAssistEnemy = Enemy;
			return;
		}
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

bool UAimComponent::ShouldActivateAimAssist(const AActor* Enemy, const FVector& CameraLocation, const FVector& CameraDirection) const
{
	if (Enemy == nullptr)
	{
		return false;
	}

	if (Enemy == GetOwner())
	{
		return false;
	}

	const float DistanceToEnemy = FVector::Dist2D(Enemy->GetActorLocation(), GetOwner()->GetActorLocation());
	if (DistanceToEnemy > AimAssistDistanceThreshold)
	{
		return false;
	}

	FVector DirectionToEnemy = Enemy->GetActorLocation() - CameraLocation;
	DirectionToEnemy.Normalize();
		
	const float DotProduct = FVector::DotProduct(DirectionToEnemy, CameraDirection);


	if (CVarDebugAimAssist->GetBool())
	{
		const float Angle = UMathUtils::GetAngleBetweenVectors(DirectionToEnemy, CameraDirection);

		FString DebugText = FString::Printf(TEXT("Distance to enemy: %.2f\nAim assist dot product: %s ° %s -> %.7f\nAngle: %.2f"), DistanceToEnemy, *DirectionToEnemy.ToString(), *CameraDirection.ToString(), DotProduct, Angle);
		FVector DebugLocation = Enemy->GetActorLocation();
		DebugLocation.Z += 100.f;
		DebugLocation.Y -= 250.f;
		DrawDebugString(GetWorld(), DebugLocation, DebugText, nullptr, FColor::Black, 0.01);
		UE_LOG(LogTemp, Error, TEXT("Distance to enemy: %.2f\nAim assist dot product: %s ° %s -> %.7f\nAngle: %.2f"), DistanceToEnemy, *DirectionToEnemy.ToString(), *CameraDirection.ToString(), DotProduct, Angle);

		DrawDebugDirectionalArrow(GetWorld(), CameraLocation, Enemy->GetActorLocation(), 5.f, FColor::Blue, false, 0.01);
		DrawDebugDirectionalArrow(GetWorld(), GetOwner()->GetActorLocation(), GetOwner()->GetActorLocation() + CameraDirection * 50000.f, 5.f, FColor::Red, false, 0.01);
	}

	if (DotProduct < AimAssistDotThreshold)
	{
		return false;
	}

	FHitResult SomethingBlockingView;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());
	Params.AddIgnoredActor(Enemy);
	const bool bHitSomething = GetWorld()->LineTraceSingleByChannel(
		SomethingBlockingView,
		GetOwner()->GetActorLocation(),
		Enemy->GetActorLocation(),
		ECC_Visibility,
		Params
	);

	if (CVarDebugAimAssist->GetBool())
	{
		DrawDebugLine(GetWorld(), GetOwner()->GetActorLocation(), Enemy->GetActorLocation(), FColor::Green, false, 0.01);
	}

	return !bHitSomething;
}

// Called every frame
void UAimComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateAimAssist();
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

